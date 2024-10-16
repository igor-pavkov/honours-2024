static NTSTATUS kdc_check_generic_kerberos(struct irpc_message *msg,
				 struct kdc_check_generic_kerberos *r)
{
	struct PAC_Validate pac_validate;
	DATA_BLOB srv_sig;
	struct PAC_SIGNATURE_DATA kdc_sig;
	struct kdc_server *kdc = talloc_get_type(msg->private_data, struct kdc_server);
	krb5_kdc_configuration *kdc_config =
		(krb5_kdc_configuration *)kdc->private_data;
	enum ndr_err_code ndr_err;
	int ret;
	hdb_entry ent;
	krb5_principal principal;


	/* There is no reply to this request */
	r->out.generic_reply = data_blob(NULL, 0);

	ndr_err = ndr_pull_struct_blob(&r->in.generic_request, msg, &pac_validate,
				       (ndr_pull_flags_fn_t)ndr_pull_PAC_Validate);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	if (pac_validate.MessageType != NETLOGON_GENERIC_KRB5_PAC_VALIDATE) {
		/* We don't implement any other message types - such as certificate validation - yet */
		return NT_STATUS_INVALID_PARAMETER;
	}

	if (pac_validate.ChecksumAndSignature.length != (pac_validate.ChecksumLength + pac_validate.SignatureLength)
	    || pac_validate.ChecksumAndSignature.length < pac_validate.ChecksumLength
	    || pac_validate.ChecksumAndSignature.length < pac_validate.SignatureLength ) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	srv_sig = data_blob_const(pac_validate.ChecksumAndSignature.data,
				  pac_validate.ChecksumLength);

	ret = krb5_make_principal(kdc->smb_krb5_context->krb5_context, &principal,
				  lpcfg_realm(kdc->task->lp_ctx),
				  "krbtgt", lpcfg_realm(kdc->task->lp_ctx),
				  NULL);

	if (ret != 0) {
		return NT_STATUS_NO_MEMORY;
	}

	ret = kdc_config->db[0]->hdb_fetch_kvno(kdc->smb_krb5_context->krb5_context,
						 kdc_config->db[0],
						 principal,
						 HDB_F_GET_KRBTGT | HDB_F_DECRYPT,
						 0,
						 &ent);

	if (ret != 0) {
		hdb_free_entry(kdc->smb_krb5_context->krb5_context, kdc_config->db[0], &ent);
		krb5_free_principal(kdc->smb_krb5_context->krb5_context, principal);

		return NT_STATUS_LOGON_FAILURE;
	}

	kdc_sig.type = pac_validate.SignatureType;
	kdc_sig.signature = data_blob_const(&pac_validate.ChecksumAndSignature.data[pac_validate.ChecksumLength],
					    pac_validate.SignatureLength);

	ret = kdc_check_pac(kdc->smb_krb5_context->krb5_context, srv_sig, &kdc_sig, &ent);

	hdb_free_entry(kdc->smb_krb5_context->krb5_context, kdc_config->db[0], &ent);
	krb5_free_principal(kdc->smb_krb5_context->krb5_context, principal);

	if (ret != 0) {
		return NT_STATUS_LOGON_FAILURE;
	}

	return NT_STATUS_OK;
}