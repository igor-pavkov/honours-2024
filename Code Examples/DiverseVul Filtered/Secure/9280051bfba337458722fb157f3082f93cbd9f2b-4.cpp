static void reply_spnego_auth(struct smb_request *req,
			      uint16 vuid,
			      DATA_BLOB blob1,
			      AUTH_NTLMSSP_STATE **auth_ntlmssp_state)
{
	DATA_BLOB auth = data_blob_null;
	DATA_BLOB auth_reply = data_blob_null;
	DATA_BLOB secblob = data_blob_null;
	NTSTATUS status = NT_STATUS_LOGON_FAILURE;
	struct smbd_server_connection *sconn = smbd_server_conn;

	if (!spnego_parse_auth(blob1, &auth)) {
#if 0
		file_save("auth.dat", blob1.data, blob1.length);
#endif
		/* Kill the intermediate vuid */
		invalidate_vuid(sconn, vuid);

		reply_nterror(req, nt_status_squash(
				      NT_STATUS_LOGON_FAILURE));
		return;
	}

	if (auth.data[0] == ASN1_APPLICATION(0)) {
		/* Might be a second negTokenTarg packet */
		char *kerb_mech = NULL;

		status = parse_spnego_mechanisms(auth, &secblob, &kerb_mech);

		if (!NT_STATUS_IS_OK(status)) {
			/* Kill the intermediate vuid */
			invalidate_vuid(sconn, vuid);
			reply_nterror(req, nt_status_squash(status));
			return;
		}

		DEBUG(3,("reply_spnego_auth: Got secblob of size %lu\n",
				(unsigned long)secblob.length));
#ifdef HAVE_KRB5
		if (kerb_mech && ((lp_security()==SEC_ADS) ||
					USE_KERBEROS_KEYTAB)) {
			bool destroy_vuid = True;
			reply_spnego_kerberos(req, &secblob, kerb_mech,
					      vuid, &destroy_vuid);
			data_blob_free(&secblob);
			data_blob_free(&auth);
			if (destroy_vuid) {
				/* Kill the intermediate vuid */
				invalidate_vuid(sconn, vuid);
			}
			SAFE_FREE(kerb_mech);
			return;
		}
#endif
		/* Can't blunder into NTLMSSP auth if we have
		 * a krb5 ticket. */

		if (kerb_mech) {
			/* Kill the intermediate vuid */
			invalidate_vuid(sconn, vuid);
			DEBUG(3,("reply_spnego_auth: network "
				"misconfiguration, client sent us a "
				"krb5 ticket and kerberos security "
				"not enabled\n"));
			reply_nterror(req, nt_status_squash(
					NT_STATUS_LOGON_FAILURE));
			SAFE_FREE(kerb_mech);
		}
	}

	/* If we get here it wasn't a negTokenTarg auth packet. */
	data_blob_free(&secblob);

	if (!*auth_ntlmssp_state) {
		status = auth_ntlmssp_start(auth_ntlmssp_state);
		if (!NT_STATUS_IS_OK(status)) {
			/* Kill the intermediate vuid */
			invalidate_vuid(sconn, vuid);
			reply_nterror(req, nt_status_squash(status));
			return;
		}
	}

	status = auth_ntlmssp_update(*auth_ntlmssp_state,
					auth, &auth_reply);

	data_blob_free(&auth);

	/* Don't send the mechid as we've already sent this (RFC4178). */

	reply_spnego_ntlmssp(req, vuid,
			     auth_ntlmssp_state,
			     &auth_reply, status, NULL, true);

	data_blob_free(&auth_reply);

	/* and tell smbd that we have already replied to this packet */
	return;
}