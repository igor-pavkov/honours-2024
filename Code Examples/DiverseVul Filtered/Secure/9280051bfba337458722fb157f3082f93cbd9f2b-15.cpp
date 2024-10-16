static void reply_spnego_negotiate(struct smb_request *req,
				   uint16 vuid,
				   DATA_BLOB blob1,
				   AUTH_NTLMSSP_STATE **auth_ntlmssp_state)
{
	DATA_BLOB secblob;
	DATA_BLOB chal;
	char *kerb_mech = NULL;
	NTSTATUS status;
	struct smbd_server_connection *sconn = smbd_server_conn;

	status = parse_spnego_mechanisms(blob1, &secblob, &kerb_mech);
	if (!NT_STATUS_IS_OK(status)) {
		/* Kill the intermediate vuid */
		invalidate_vuid(sconn, vuid);
		reply_nterror(req, nt_status_squash(status));
		return;
	}

	DEBUG(3,("reply_spnego_negotiate: Got secblob of size %lu\n",
				(unsigned long)secblob.length));

#ifdef HAVE_KRB5
	if (kerb_mech && ((lp_security()==SEC_ADS) ||
				USE_KERBEROS_KEYTAB) ) {
		bool destroy_vuid = True;
		reply_spnego_kerberos(req, &secblob, kerb_mech,
				      vuid, &destroy_vuid);
		data_blob_free(&secblob);
		if (destroy_vuid) {
			/* Kill the intermediate vuid */
			invalidate_vuid(sconn, vuid);
		}
		SAFE_FREE(kerb_mech);
		return;
	}
#endif

	if (*auth_ntlmssp_state) {
		auth_ntlmssp_end(auth_ntlmssp_state);
	}

	if (kerb_mech) {
		data_blob_free(&secblob);
		/* The mechtoken is a krb5 ticket, but
		 * we need to fall back to NTLM. */
		reply_spnego_downgrade_to_ntlmssp(req, vuid);
		SAFE_FREE(kerb_mech);
		return;
	}

	status = auth_ntlmssp_start(auth_ntlmssp_state);
	if (!NT_STATUS_IS_OK(status)) {
		/* Kill the intermediate vuid */
		invalidate_vuid(sconn, vuid);
		reply_nterror(req, nt_status_squash(status));
		return;
	}

	status = auth_ntlmssp_update(*auth_ntlmssp_state,
					secblob, &chal);

	data_blob_free(&secblob);

	reply_spnego_ntlmssp(req, vuid, auth_ntlmssp_state,
			     &chal, status, OID_NTLMSSP, true);

	data_blob_free(&chal);

	/* already replied */
	return;
}