static SMB_ACL_T vfswrap_sys_acl_get_file(vfs_handle_struct *handle,
					  const char *path_p,
					  SMB_ACL_TYPE_T type,
					  TALLOC_CTX *mem_ctx)
{
	return sys_acl_get_file(handle, path_p, type, mem_ctx);
}