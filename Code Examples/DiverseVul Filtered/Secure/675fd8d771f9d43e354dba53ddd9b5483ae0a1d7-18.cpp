NTSTATUS vfs_shadow_copy2_init(void)
{
	return smb_register_vfs(SMB_VFS_INTERFACE_VERSION,
				"shadow_copy2", &vfs_shadow_copy2_fns);
}