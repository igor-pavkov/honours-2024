static NTSTATUS vfswrap_fget_nt_acl(vfs_handle_struct *handle,
				    files_struct *fsp,
				    uint32 security_info,
				    TALLOC_CTX *mem_ctx,
				    struct security_descriptor **ppdesc)
{
	NTSTATUS result;

	START_PROFILE(fget_nt_acl);
	result = posix_fget_nt_acl(fsp, security_info,
				   mem_ctx, ppdesc);
	END_PROFILE(fget_nt_acl);
	return result;
}