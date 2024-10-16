static int vfswrap_sys_acl_set_file(vfs_handle_struct *handle, const char *name, SMB_ACL_TYPE_T acltype, SMB_ACL_T theacl)
{
	return sys_acl_set_file(handle, name, acltype, theacl);
}