static DIR *shadow_copy2_opendir(vfs_handle_struct *handle,
					    const char *fname,
					    const char *mask,
					    uint32_t attr)
{
	time_t timestamp;
	char *stripped;
	DIR *ret;
	int saved_errno;
	char *conv;

	if (!shadow_copy2_strip_snapshot(talloc_tos(), handle, fname,
					 &timestamp, &stripped)) {
		return NULL;
	}
	if (timestamp == 0) {
		return SMB_VFS_NEXT_OPENDIR(handle, fname, mask, attr);
	}
	conv = shadow_copy2_convert(talloc_tos(), handle, stripped, timestamp);
	TALLOC_FREE(stripped);
	if (conv == NULL) {
		return NULL;
	}
	ret = SMB_VFS_NEXT_OPENDIR(handle, conv, mask, attr);
	saved_errno = errno;
	TALLOC_FREE(conv);
	errno = saved_errno;
	return ret;
}