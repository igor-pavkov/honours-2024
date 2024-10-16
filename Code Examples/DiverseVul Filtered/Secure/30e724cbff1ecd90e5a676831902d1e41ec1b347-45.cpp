static NTSTATUS vfswrap_get_nt_acl(vfs_handle_struct *handle,
				   const char *name,
				   uint32 security_info,
				   TALLOC_CTX *mem_ctx,
				   struct security_descriptor **ppdesc)
{
	NTSTATUS result;

	START_PROFILE(get_nt_acl);
	result = posix_get_nt_acl(handle->conn, name, security_info,
				  mem_ctx, ppdesc);
	END_PROFILE(get_nt_acl);
	return result;
}