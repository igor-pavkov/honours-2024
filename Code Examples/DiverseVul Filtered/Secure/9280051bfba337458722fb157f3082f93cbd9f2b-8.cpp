static void reply_spnego_kerberos(struct smb_request *req,
				  DATA_BLOB *secblob,
				  const char *mechOID,
				  uint16 vuid,
				  bool *p_invalidate_vuid)
{
	TALLOC_CTX *mem_ctx;
	DATA_BLOB ticket;
	char *client, *p, *domain;
	fstring netbios_domain_name;
	struct passwd *pw;
	fstring user;
	int sess_vuid = req->vuid;
	NTSTATUS ret = NT_STATUS_OK;
	struct PAC_DATA *pac_data = NULL;
	DATA_BLOB ap_rep, ap_rep_wrapped, response;
	auth_serversupplied_info *server_info = NULL;
	DATA_BLOB session_key = data_blob_null;
	uint8 tok_id[2];
	DATA_BLOB nullblob = data_blob_null;
	fstring real_username;
	bool map_domainuser_to_guest = False;
	bool username_was_mapped;
	struct PAC_LOGON_INFO *logon_info = NULL;
	struct smbd_server_connection *sconn = smbd_server_conn;

	ZERO_STRUCT(ticket);
	ZERO_STRUCT(ap_rep);
	ZERO_STRUCT(ap_rep_wrapped);
	ZERO_STRUCT(response);

	/* Normally we will always invalidate the intermediate vuid. */
	*p_invalidate_vuid = True;

	mem_ctx = talloc_init("reply_spnego_kerberos");
	if (mem_ctx == NULL) {
		reply_nterror(req, nt_status_squash(NT_STATUS_NO_MEMORY));
		return;
	}

	if (!spnego_parse_krb5_wrap(*secblob, &ticket, tok_id)) {
		talloc_destroy(mem_ctx);
		reply_nterror(req, nt_status_squash(NT_STATUS_LOGON_FAILURE));
		return;
	}

	ret = ads_verify_ticket(mem_ctx, lp_realm(), 0, &ticket,
				&client, &pac_data, &ap_rep,
				&session_key, True);

	data_blob_free(&ticket);

	if (!NT_STATUS_IS_OK(ret)) {
#if 0
		/* Experiment that failed.
		 * See "only happens with a KDC" comment below. */

		if (NT_STATUS_EQUAL(ret, NT_STATUS_TIME_DIFFERENCE_AT_DC)) {

			/*
			 * Windows in this case returns
			 * NT_STATUS_MORE_PROCESSING_REQUIRED
			 * with a negTokenTarg blob containing an krb5_error
			 * struct ASN1 encoded containing KRB5KRB_AP_ERR_SKEW.
			 * The client then fixes its clock and continues rather
			 * than giving an error. JRA.
			 * -- Looks like this only happens with a KDC. JRA.
			 */

			bool ok = make_krb5_skew_error(&ap_rep);
			if (!ok) {
				talloc_destroy(mem_ctx);
				return ERROR_NT(nt_status_squash(
						NT_STATUS_LOGON_FAILURE));
			}
			ap_rep_wrapped = spnego_gen_krb5_wrap(ap_rep,
					TOK_ID_KRB_ERROR);
			response = spnego_gen_auth_response(&ap_rep_wrapped,
					ret, OID_KERBEROS5_OLD);
			reply_sesssetup_blob(conn, inbuf, outbuf, response,
					NT_STATUS_MORE_PROCESSING_REQUIRED);

			/*
			 * In this one case we don't invalidate the
			 * intermediate vuid as we're expecting the client
			 * to re-use it for the next sessionsetupX packet. JRA.
			 */

			*p_invalidate_vuid = False;

			data_blob_free(&ap_rep);
			data_blob_free(&ap_rep_wrapped);
			data_blob_free(&response);
			talloc_destroy(mem_ctx);
			return -1; /* already replied */
		}
#else
		if (!NT_STATUS_EQUAL(ret, NT_STATUS_TIME_DIFFERENCE_AT_DC)) {
			ret = NT_STATUS_LOGON_FAILURE;
		}
#endif
		DEBUG(1,("Failed to verify incoming ticket with error %s!\n",
				nt_errstr(ret)));
		talloc_destroy(mem_ctx);
		reply_nterror(req, nt_status_squash(ret));
		return;
	}

	DEBUG(3,("Ticket name is [%s]\n", client));

	p = strchr_m(client, '@');
	if (!p) {
		DEBUG(3,("Doesn't look like a valid principal\n"));
		data_blob_free(&ap_rep);
		data_blob_free(&session_key);
		talloc_destroy(mem_ctx);
		reply_nterror(req,nt_status_squash(NT_STATUS_LOGON_FAILURE));
		return;
	}

	*p = 0;

	/* save the PAC data if we have it */

	if (pac_data) {
		logon_info = get_logon_info_from_pac(pac_data);
		if (logon_info) {
			netsamlogon_cache_store( client, &logon_info->info3 );
		}
	}

	if (!strequal(p+1, lp_realm())) {
		DEBUG(3,("Ticket for foreign realm %s@%s\n", client, p+1));
		if (!lp_allow_trusted_domains()) {
			data_blob_free(&ap_rep);
			data_blob_free(&session_key);
			talloc_destroy(mem_ctx);
			reply_nterror(req, nt_status_squash(
					      NT_STATUS_LOGON_FAILURE));
			return;
		}
	}

	/* this gives a fully qualified user name (ie. with full realm).
	   that leads to very long usernames, but what else can we do? */

	domain = p+1;

	if (logon_info && logon_info->info3.base.domain.string) {
		fstrcpy(netbios_domain_name,
			logon_info->info3.base.domain.string);
		domain = netbios_domain_name;
		DEBUG(10, ("Mapped to [%s] (using PAC)\n", domain));

	} else {

		/* If we have winbind running, we can (and must) shorten the
		   username by using the short netbios name. Otherwise we will
		   have inconsistent user names. With Kerberos, we get the
		   fully qualified realm, with ntlmssp we get the short
		   name. And even w2k3 does use ntlmssp if you for example
		   connect to an ip address. */

		wbcErr wbc_status;
		struct wbcDomainInfo *info = NULL;

		DEBUG(10, ("Mapping [%s] to short name\n", domain));

		wbc_status = wbcDomainInfo(domain, &info);

		if (WBC_ERROR_IS_OK(wbc_status)) {

			fstrcpy(netbios_domain_name,
				info->short_name);

			wbcFreeMemory(info);
			domain = netbios_domain_name;
			DEBUG(10, ("Mapped to [%s] (using Winbind)\n", domain));
		} else {
			DEBUG(3, ("Could not find short name: %s\n",
				wbcErrorString(wbc_status)));
		}
	}

	fstr_sprintf(user, "%s%c%s", domain, *lp_winbind_separator(), client);

	/* lookup the passwd struct, create a new user if necessary */

	username_was_mapped = map_username(sconn, user);

	pw = smb_getpwnam( mem_ctx, user, real_username, True );

	if (pw) {
		/* if a real user check pam account restrictions */
		/* only really perfomed if "obey pam restriction" is true */
		/* do this before an eventual mapping to guest occurs */
		ret = smb_pam_accountcheck(pw->pw_name);
		if (  !NT_STATUS_IS_OK(ret)) {
			DEBUG(1,("PAM account restriction "
				"prevents user login\n"));
			data_blob_free(&ap_rep);
			data_blob_free(&session_key);
			TALLOC_FREE(mem_ctx);
			reply_nterror(req, nt_status_squash(ret));
			return;
		}
	}

	if (!pw) {

		/* this was originally the behavior of Samba 2.2, if a user
		   did not have a local uid but has been authenticated, then
		   map them to a guest account */

		if (lp_map_to_guest() == MAP_TO_GUEST_ON_BAD_UID){
			map_domainuser_to_guest = True;
			fstrcpy(user,lp_guestaccount());
			pw = smb_getpwnam( mem_ctx, user, real_username, True );
		}

		/* extra sanity check that the guest account is valid */

		if ( !pw ) {
			DEBUG(1,("Username %s is invalid on this system\n",
				user));
			data_blob_free(&ap_rep);
			data_blob_free(&session_key);
			TALLOC_FREE(mem_ctx);
			reply_nterror(req, nt_status_squash(
					      NT_STATUS_LOGON_FAILURE));
			return;
		}
	}

	/* setup the string used by %U */

	sub_set_smb_name( real_username );
	reload_services(True);

	if ( map_domainuser_to_guest ) {
		make_server_info_guest(NULL, &server_info);
	} else if (logon_info) {
		/* pass the unmapped username here since map_username()
		   will be called again from inside make_server_info_info3() */

		ret = make_server_info_info3(mem_ctx, client, domain,
					     &server_info, &logon_info->info3);
		if ( !NT_STATUS_IS_OK(ret) ) {
			DEBUG(1,("make_server_info_info3 failed: %s!\n",
				 nt_errstr(ret)));
			data_blob_free(&ap_rep);
			data_blob_free(&session_key);
			TALLOC_FREE(mem_ctx);
			reply_nterror(req, nt_status_squash(ret));
			return;
		}

	} else {
		/*
		 * We didn't get a PAC, we have to make up the user
		 * ourselves. Try to ask the pdb backend to provide
		 * SID consistency with ntlmssp session setup
		 */
		struct samu *sampass;

		sampass = samu_new(talloc_tos());
		if (sampass == NULL) {
			ret = NT_STATUS_NO_MEMORY;
			data_blob_free(&ap_rep);
			data_blob_free(&session_key);
			TALLOC_FREE(mem_ctx);
			reply_nterror(req, nt_status_squash(ret));
			return;
		}

		if (pdb_getsampwnam(sampass, real_username)) {
			DEBUG(10, ("found user %s in passdb, calling "
				   "make_server_info_sam\n", real_username));
			ret = make_server_info_sam(&server_info, sampass);
		} else {
			/*
			 * User not in passdb, make it up artificially
			 */
			TALLOC_FREE(sampass);
			DEBUG(10, ("didn't find user %s in passdb, calling "
				   "make_server_info_pw\n", real_username));
			ret = make_server_info_pw(&server_info, real_username,
						  pw);
		}

		if ( !NT_STATUS_IS_OK(ret) ) {
			DEBUG(1,("make_server_info_[sam|pw] failed: %s!\n",
				 nt_errstr(ret)));
			data_blob_free(&ap_rep);
			data_blob_free(&session_key);
			TALLOC_FREE(mem_ctx);
			reply_nterror(req, nt_status_squash(ret));
			return;
		}

	        /* make_server_info_pw does not set the domain. Without this
		 * we end up with the local netbios name in substitutions for
		 * %D. */

		if (server_info->sam_account != NULL) {
			pdb_set_domain(server_info->sam_account,
					domain, PDB_SET);
		}
	}

	server_info->nss_token |= username_was_mapped;

	/* we need to build the token for the user. make_server_info_guest()
	   already does this */

	if ( !server_info->ptok ) {
		ret = create_local_token( server_info );
		if ( !NT_STATUS_IS_OK(ret) ) {
			DEBUG(10,("failed to create local token: %s\n",
				nt_errstr(ret)));
			data_blob_free(&ap_rep);
			data_blob_free(&session_key);
			TALLOC_FREE( mem_ctx );
			TALLOC_FREE( server_info );
			reply_nterror(req, nt_status_squash(ret));
			return;
		}
	}

	if (!is_partial_auth_vuid(sconn, sess_vuid)) {
		sess_vuid = register_initial_vuid(sconn);
	}

	data_blob_free(&server_info->user_session_key);
	server_info->user_session_key = session_key;
	session_key = data_blob_null;

	/* register_existing_vuid keeps the server info */
	/* register_existing_vuid takes ownership of session_key on success,
	 * no need to free after this on success. A better interface would copy
	 * it.... */

	sess_vuid = register_existing_vuid(sconn,
					sess_vuid,
					server_info,
					nullblob,
					client);

	reply_outbuf(req, 4, 0);
	SSVAL(req->outbuf,smb_uid,sess_vuid);

	if (sess_vuid == UID_FIELD_INVALID ) {
		ret = NT_STATUS_LOGON_FAILURE;
	} else {
		/* current_user_info is changed on new vuid */
		reload_services( True );

		SSVAL(req->outbuf, smb_vwv3, 0);

		if (server_info->guest) {
			SSVAL(req->outbuf,smb_vwv2,1);
		}

		SSVAL(req->outbuf, smb_uid, sess_vuid);

		/* Successful logon. Keep this vuid. */
		*p_invalidate_vuid = False;
	}

        /* wrap that up in a nice GSS-API wrapping */
	if (NT_STATUS_IS_OK(ret)) {
		ap_rep_wrapped = spnego_gen_krb5_wrap(ap_rep,
				TOK_ID_KRB_AP_REP);
	} else {
		ap_rep_wrapped = data_blob_null;
	}
	response = spnego_gen_auth_response(&ap_rep_wrapped, ret,
			mechOID);
	reply_sesssetup_blob(req, response, ret);

	data_blob_free(&ap_rep);
	data_blob_free(&ap_rep_wrapped);
	data_blob_free(&response);
	TALLOC_FREE(mem_ctx);
}