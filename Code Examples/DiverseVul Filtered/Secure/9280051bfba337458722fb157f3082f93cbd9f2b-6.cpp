static void reply_sesssetup_blob(struct smb_request *req,
				 DATA_BLOB blob,
				 NTSTATUS nt_status)
{
	if (!NT_STATUS_IS_OK(nt_status) &&
	    !NT_STATUS_EQUAL(nt_status, NT_STATUS_MORE_PROCESSING_REQUIRED)) {
		reply_nterror(req, nt_status_squash(nt_status));
		return;
	}

	nt_status = nt_status_squash(nt_status);
	SIVAL(req->outbuf, smb_rcls, NT_STATUS_V(nt_status));
	SSVAL(req->outbuf, smb_vwv0, 0xFF); /* no chaining possible */
	SSVAL(req->outbuf, smb_vwv3, blob.length);

	if ((message_push_blob(&req->outbuf, blob) == -1)
	    || (push_signature(&req->outbuf) == -1)) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
	}
}