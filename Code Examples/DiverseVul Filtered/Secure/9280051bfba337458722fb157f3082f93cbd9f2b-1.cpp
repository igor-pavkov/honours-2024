static void reply_spnego_downgrade_to_ntlmssp(struct smb_request *req,
						uint16 vuid)
{
	DATA_BLOB response;

	reply_outbuf(req, 4, 0);
        SSVAL(req->outbuf,smb_uid,vuid);

	DEBUG(3,("reply_spnego_downgrade_to_ntlmssp: Got krb5 ticket in SPNEGO "
		"but set to downgrade to NTLMSSP\n"));

	response = spnego_gen_auth_response(NULL,
			NT_STATUS_MORE_PROCESSING_REQUIRED,
			OID_NTLMSSP);
	reply_sesssetup_blob(req, response, NT_STATUS_MORE_PROCESSING_REQUIRED);
	data_blob_free(&response);
}