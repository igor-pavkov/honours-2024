static int vfswrap_sys_acl_set_fd(vfs_handle_struct *handle, files_struct *fsp, SMB_ACL_T theacl)
{
	return sys_acl_set_fd(handle, fsp, theacl);
}