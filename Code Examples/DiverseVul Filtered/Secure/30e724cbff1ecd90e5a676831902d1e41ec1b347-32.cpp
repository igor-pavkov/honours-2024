static NTSTATUS vfswrap_fset_nt_acl(vfs_handle_struct *handle, files_struct *fsp, uint32 security_info_sent, const struct security_descriptor *psd)
{
	NTSTATUS result;

	START_PROFILE(fset_nt_acl);
	result = set_nt_acl(fsp, security_info_sent, psd);
	END_PROFILE(fset_nt_acl);
	return result;
}