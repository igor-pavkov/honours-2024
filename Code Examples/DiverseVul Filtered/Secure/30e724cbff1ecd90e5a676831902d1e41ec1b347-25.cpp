static NTSTATUS vfswrap_durable_cookie(struct vfs_handle_struct *handle,
				       struct files_struct *fsp,
				       TALLOC_CTX *mem_ctx,
				       DATA_BLOB *cookie)
{
	return vfs_default_durable_cookie(fsp, mem_ctx, cookie);
}