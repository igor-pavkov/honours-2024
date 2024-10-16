static void smbd_smb1_notify_reply(struct smb_request *req,
				   NTSTATUS error_code,
				   uint8_t *buf, size_t len)
{
	send_nt_replies(req->conn, req, error_code, (char *)buf, len, NULL, 0);
}