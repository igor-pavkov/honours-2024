static int vfswrap_sys_acl_delete_def_file(vfs_handle_struct *handle, const char *path)
{
	return sys_acl_delete_def_file(handle, path);
}