NTSTATUS vfs_default_init(void)
{
	return smb_register_vfs(SMB_VFS_INTERFACE_VERSION,
				DEFAULT_VFS_MODULE_NAME, &vfs_default_fns);
}