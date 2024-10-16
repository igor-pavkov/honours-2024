static void reply_spnego_ntlmssp(struct smb_request *req,
				 uint16 vuid,
				 AUTH_NTLMSSP_STATE **auth_ntlmssp_state,
				 DATA_BLOB *ntlmssp_blob, NTSTATUS nt_status,
				 const char *OID,
				 bool wrap)
{
	DATA_BLOB response;
	struct auth_serversupplied_info *server_info = NULL;
	struct smbd_server_connection *sconn = smbd_server_conn;

	if (NT_STATUS_IS_OK(nt_status)) {
		server_info = (*auth_ntlmssp_state)->server_info;
	} else {
		nt_status = do_map_to_guest(nt_status,
			    &server_info,
			    (*auth_ntlmssp_state)->ntlmssp_state->user,
			    (*auth_ntlmssp_state)->ntlmssp_state->domain);
	}

	reply_outbuf(req, 4, 0);

	SSVAL(req->outbuf, smb_uid, vuid);

	if (NT_STATUS_IS_OK(nt_status)) {
		DATA_BLOB nullblob = data_blob_null;

		if (!is_partial_auth_vuid(sconn, vuid)) {
			nt_status = NT_STATUS_LOGON_FAILURE;
			goto out;
		}

		data_blob_free(&server_info->user_session_key);
		server_info->user_session_key =
			data_blob_talloc(
			server_info,
			(*auth_ntlmssp_state)->ntlmssp_state->session_key.data,
			(*auth_ntlmssp_state)->ntlmssp_state->session_key.length);

		/* register_existing_vuid keeps the server info */
		if (register_existing_vuid(sconn, vuid,
				server_info, nullblob,
				(*auth_ntlmssp_state)->ntlmssp_state->user) !=
					vuid) {
			nt_status = NT_STATUS_LOGON_FAILURE;
			goto out;
		}

		(*auth_ntlmssp_state)->server_info = NULL;

		/* current_user_info is changed on new vuid */
		reload_services( True );

		SSVAL(req->outbuf, smb_vwv3, 0);

		if (server_info->guest) {
			SSVAL(req->outbuf,smb_vwv2,1);
		}
	}

  out:

	if (wrap) {
		response = spnego_gen_auth_response(ntlmssp_blob,
				nt_status, OID);
	} else {
		response = *ntlmssp_blob;
	}

	reply_sesssetup_blob(req, response, nt_status);
	if (wrap) {
		data_blob_free(&response);
	}

	/* NT_STATUS_MORE_PROCESSING_REQUIRED from our NTLMSSP code tells us,
	   and the other end, that we are not finished yet. */

	if (!NT_STATUS_EQUAL(nt_status, NT_STATUS_MORE_PROCESSING_REQUIRED)) {
		/* NB. This is *NOT* an error case. JRA */
		auth_ntlmssp_end(auth_ntlmssp_state);
		if (!NT_STATUS_IS_OK(nt_status)) {
			/* Kill the intermediate vuid */
			invalidate_vuid(sconn, vuid);
		}
	}
}