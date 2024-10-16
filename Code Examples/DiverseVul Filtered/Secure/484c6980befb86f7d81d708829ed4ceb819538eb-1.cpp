static krb5_error_code hdb_samba4_audit(krb5_context context,
					HDB *db,
					hdb_entry *entry,
					hdb_request_t r)
{
	struct samba_kdc_db_context *kdc_db_ctx = talloc_get_type_abort(db->hdb_db,
									struct samba_kdc_db_context);
	struct ldb_dn *domain_dn = ldb_get_default_basedn(kdc_db_ctx->samdb);
	uint64_t logon_id = generate_random_u64();
	heim_object_t auth_details_obj = NULL;
	const char *auth_details = NULL;
	char *etype_str = NULL;
	heim_object_t hdb_auth_status_obj = NULL;
	int hdb_auth_status;
	heim_object_t pa_type_obj = NULL;
	const char *pa_type = NULL;
	struct auth_usersupplied_info ui;
	size_t sa_socklen = 0;
	int final_ret = 0;

	hdb_auth_status_obj = heim_audit_getkv((heim_svc_req_desc)r, KDC_REQUEST_KV_AUTH_EVENT);
	if (hdb_auth_status_obj == NULL) {
		/* No status code found, so just return. */
		return 0;
	}

	hdb_auth_status = heim_number_get_int(hdb_auth_status_obj);

	pa_type_obj = heim_audit_getkv((heim_svc_req_desc)r, "pa");
	if (pa_type_obj != NULL) {
		pa_type = heim_string_get_utf8(pa_type_obj);
	}

	auth_details_obj = heim_audit_getkv((heim_svc_req_desc)r, KDC_REQUEST_KV_PKINIT_CLIENT_CERT);
	if (auth_details_obj != NULL) {
		auth_details = heim_string_get_utf8(auth_details_obj);
	} else {
		auth_details_obj = heim_audit_getkv((heim_svc_req_desc)r, KDC_REQUEST_KV_GSS_INITIATOR);
		if (auth_details_obj != NULL) {
			auth_details = heim_string_get_utf8(auth_details_obj);
		} else {
			heim_object_t etype_obj = heim_audit_getkv((heim_svc_req_desc)r, KDC_REQUEST_KV_PA_ETYPE);
			if (etype_obj != NULL) {
				int etype = heim_number_get_int(etype_obj);

				krb5_error_code ret = krb5_enctype_to_string(r->context, etype, &etype_str);
				if (ret == 0) {
					auth_details = etype_str;
				} else {
					auth_details = "unknown enctype";
				}
			}
		}
	}

	/*
	 * Forcing this via the NTLM auth structure is not ideal, but
	 * it is the most practical option right now, and ensures the
	 * logs are consistent, even if some elements are always NULL.
	 */
	ui = (struct auth_usersupplied_info) {
		.was_mapped = true,
		.client = {
			.account_name = r->cname,
			.domain_name = NULL,
		},
		.service_description = "Kerberos KDC",
		.auth_description = "Unknown Auth Description",
		.password_type = auth_details,
		.logon_id = logon_id
	};

	switch (r->addr->sa_family) {
	case AF_INET:
		sa_socklen = sizeof(struct sockaddr_in);
		break;
#ifdef HAVE_IPV6
	case AF_INET6:
		sa_socklen = sizeof(struct sockaddr_in6);
		break;
#endif
	}

	switch (hdb_auth_status) {
	case KDC_AUTH_EVENT_CLIENT_AUTHORIZED:
	{
		TALLOC_CTX *frame = talloc_stackframe();
		struct samba_kdc_entry *p = talloc_get_type(entry->context,
							    struct samba_kdc_entry);
		struct netr_SendToSamBase *send_to_sam = NULL;

		/*
		 * TODO: We could log the AS-REQ authorization success here as
		 * well.  However before we do that, we need to pass
		 * in the PAC here or re-calculate it.
		 */
		authsam_logon_success_accounting(kdc_db_ctx->samdb, p->msg,
						 domain_dn, true, &send_to_sam);
		if (kdc_db_ctx->rodc && send_to_sam != NULL) {
			reset_bad_password_netlogon(frame, kdc_db_ctx, send_to_sam);
		}
		talloc_free(frame);
	}
	FALL_THROUGH;
	default:
	{
		TALLOC_CTX *frame = talloc_stackframe();
		struct samba_kdc_entry *p = talloc_get_type(entry->context,
							    struct samba_kdc_entry);
		struct dom_sid *sid
			= samdb_result_dom_sid(frame, p->msg, "objectSid");
		const char *account_name
			= ldb_msg_find_attr_as_string(p->msg, "sAMAccountName", NULL);
		const char *domain_name = lpcfg_sam_name(p->kdc_db_ctx->lp_ctx);
		struct tsocket_address *remote_host;
		const char *auth_description = NULL;
		NTSTATUS status;
		int ret;
		bool rwdc_fallback = false;

		ret = tsocket_address_bsd_from_sockaddr(frame, r->addr,
							sa_socklen,
							&remote_host);
		if (ret != 0) {
			ui.remote_host = NULL;
		} else {
			ui.remote_host = remote_host;
		}

		ui.mapped.account_name = account_name;
		ui.mapped.domain_name = domain_name;

		if (pa_type != NULL) {
			auth_description = talloc_asprintf(frame,
							   "%s Pre-authentication",
							   pa_type);
			if (auth_description == NULL) {
				auth_description = pa_type;
			}
		} else {
			auth_description = "Unknown Pre-authentication";
		}
		ui.auth_description = auth_description;

		if (hdb_auth_status == KDC_AUTH_EVENT_CLIENT_AUTHORIZED) {
			/* This is the final sucess */
			status = NT_STATUS_OK;
		} else if (hdb_auth_status == KDC_AUTH_EVENT_VALIDATED_LONG_TERM_KEY) {
			/*
			 * This was only a pre-authentication success,
			 * but we didn't reach the final
			 * KDC_AUTH_EVENT_CLIENT_AUTHORIZED,
			 * so consult the error code.
			 */
			if (r->error_code == 0) {
				DBG_ERR("ERROR: VALIDATED_LONG_TERM_KEY "
					"with error=0 => INTERNAL_ERROR\n");
				status = NT_STATUS_INTERNAL_ERROR;
				final_ret = KRB5KRB_ERR_GENERIC;
				r->error_code = final_ret;
			} else if (!NT_STATUS_IS_OK(p->reject_status)) {
				status = p->reject_status;
			} else {
				status = krb5_to_nt_status(r->error_code);
			}
		} else if (hdb_auth_status == KDC_AUTH_EVENT_PREAUTH_SUCCEEDED) {
			/*
			 * This was only a pre-authentication success,
			 * but we didn't reach the final
			 * KDC_AUTH_EVENT_CLIENT_AUTHORIZED,
			 * so consult the error code.
			 */
			if (r->error_code == 0) {
				DBG_ERR("ERROR: PREAUTH_SUCCEEDED "
					"with error=0 => INTERNAL_ERROR\n");
				status = NT_STATUS_INTERNAL_ERROR;
				final_ret = KRB5KRB_ERR_GENERIC;
				r->error_code = final_ret;
			} else if (!NT_STATUS_IS_OK(p->reject_status)) {
				status = p->reject_status;
			} else {
				status = krb5_to_nt_status(r->error_code);
			}
		} else if (hdb_auth_status == KDC_AUTH_EVENT_CLIENT_TIME_SKEW) {
			status = NT_STATUS_TIME_DIFFERENCE_AT_DC;
		} else if (hdb_auth_status == KDC_AUTH_EVENT_WRONG_LONG_TERM_KEY) {
			authsam_update_bad_pwd_count(kdc_db_ctx->samdb, p->msg, domain_dn);
			status = NT_STATUS_WRONG_PASSWORD;
			rwdc_fallback = kdc_db_ctx->rodc;
		} else if (hdb_auth_status == KDC_AUTH_EVENT_CLIENT_LOCKED_OUT) {
			status = NT_STATUS_ACCOUNT_LOCKED_OUT;
			rwdc_fallback = kdc_db_ctx->rodc;
		} else if (hdb_auth_status == KDC_AUTH_EVENT_CLIENT_NAME_UNAUTHORIZED) {
			if (pa_type != NULL && strncmp(pa_type, "PK-INIT", strlen("PK-INIT")) == 0) {
				status = NT_STATUS_PKINIT_NAME_MISMATCH;
			} else {
				status = NT_STATUS_ACCOUNT_RESTRICTION;
			}
			rwdc_fallback = kdc_db_ctx->rodc;
		} else if (hdb_auth_status == KDC_AUTH_EVENT_PREAUTH_FAILED) {
			if (pa_type != NULL && strncmp(pa_type, "PK-INIT", strlen("PK-INIT")) == 0) {
				status = NT_STATUS_PKINIT_FAILURE;
			} else {
				status = NT_STATUS_GENERIC_COMMAND_FAILED;
			}
			rwdc_fallback = kdc_db_ctx->rodc;
		} else {
			DBG_ERR("Unhandled hdb_auth_status=%d => INTERNAL_ERROR\n",
				hdb_auth_status);
			status = NT_STATUS_INTERNAL_ERROR;
			final_ret = KRB5KRB_ERR_GENERIC;
			r->error_code = final_ret;
		}

		if (rwdc_fallback) {
			/*
			 * Forward the request to an RWDC in order
			 * to give an authoritative answer to the client.
			 */
			auth_description = talloc_asprintf(frame,
							   "%s,Forward-To-RWDC",
							   ui.auth_description);
			if (auth_description != NULL) {
				ui.auth_description = auth_description;
			}
			final_ret = HDB_ERR_NOT_FOUND_HERE;
		}

		log_authentication_event(kdc_db_ctx->msg_ctx,
					 kdc_db_ctx->lp_ctx,
					 &r->tv_start,
					 &ui,
					 status,
					 domain_name,
					 account_name,
					 sid);
		if (final_ret == KRB5KRB_ERR_GENERIC && socket_wrapper_enabled()) {
			/*
			 * If we're running under make test
			 * just panic
			 */
			DBG_ERR("Unexpected situation => PANIC\n");
			smb_panic("hdb_samba4_audit: Unexpected situation");
		}
		TALLOC_FREE(frame);
		break;
	}
	case KDC_AUTH_EVENT_CLIENT_UNKNOWN:
	{
		struct tsocket_address *remote_host;
		int ret;
		TALLOC_CTX *frame = talloc_stackframe();
		ret = tsocket_address_bsd_from_sockaddr(frame, r->addr,
							sa_socklen,
							&remote_host);
		if (ret != 0) {
			ui.remote_host = NULL;
		} else {
			ui.remote_host = remote_host;
		}

		if (pa_type == NULL) {
			pa_type = "AS-REQ";
		}

		ui.auth_description = pa_type;

		/* Note this is not forwarded to an RWDC */

		log_authentication_event(kdc_db_ctx->msg_ctx,
					 kdc_db_ctx->lp_ctx,
					 &r->tv_start,
					 &ui,
					 NT_STATUS_NO_SUCH_USER,
					 NULL, NULL,
					 NULL);
		TALLOC_FREE(frame);
		break;
	}
	}

	free(etype_str);

	return final_ret;
}