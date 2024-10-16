static NTSTATUS vfswrap_durable_reconnect(struct vfs_handle_struct *handle,
					  struct smb_request *smb1req,
					  struct smbXsrv_open *op,
					  const DATA_BLOB old_cookie,
					  TALLOC_CTX *mem_ctx,
					  struct files_struct **fsp,
					  DATA_BLOB *new_cookie)
{
	return vfs_default_durable_reconnect(handle->conn, smb1req, op,
					     old_cookie, mem_ctx,
					     fsp, new_cookie);
}