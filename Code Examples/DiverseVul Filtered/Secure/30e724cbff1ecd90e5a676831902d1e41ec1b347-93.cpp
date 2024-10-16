static NTSTATUS vfswrap_durable_disconnect(struct vfs_handle_struct *handle,
					   struct files_struct *fsp,
					   const DATA_BLOB old_cookie,
					   TALLOC_CTX *mem_ctx,
					   DATA_BLOB *new_cookie)
{
	return vfs_default_durable_disconnect(fsp, old_cookie, mem_ctx,
					      new_cookie);
}