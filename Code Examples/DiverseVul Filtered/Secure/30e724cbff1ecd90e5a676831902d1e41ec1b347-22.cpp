static SMB_ACL_T vfswrap_sys_acl_get_fd(vfs_handle_struct *handle,
					files_struct *fsp,
					TALLOC_CTX *mem_ctx)
{
	return sys_acl_get_fd(handle, fsp, mem_ctx);
}