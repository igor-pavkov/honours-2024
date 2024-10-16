static NTSTATUS shadow_copy2_get_nt_acl(vfs_handle_struct *handle,
					const char *fname,
					uint32_t security_info,
					TALLOC_CTX *mem_ctx,
					struct security_descriptor **ppdesc)
{
	time_t timestamp;
	char *stripped;
	NTSTATUS status;
	char *conv;

	if (!shadow_copy2_strip_snapshot(talloc_tos(), handle, fname,
					 &timestamp, &stripped)) {
		return map_nt_error_from_unix(errno);
	}
	if (timestamp == 0) {
		return SMB_VFS_NEXT_GET_NT_ACL(handle, fname, security_info,
					       mem_ctx, ppdesc);
	}
	conv = shadow_copy2_convert(talloc_tos(), handle, stripped, timestamp);
	TALLOC_FREE(stripped);
	if (conv == NULL) {
		return map_nt_error_from_unix(errno);
	}
	status = SMB_VFS_NEXT_GET_NT_ACL(handle, conv, security_info,
					 mem_ctx, ppdesc);
	TALLOC_FREE(conv);
	return status;
}