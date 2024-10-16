static int shadow_copy2_chown(vfs_handle_struct *handle, const char *fname,
			      uid_t uid, gid_t gid)
{
	time_t timestamp;
	char *stripped;
	int ret, saved_errno;
	char *conv;

	if (!shadow_copy2_strip_snapshot(talloc_tos(), handle, fname,
					 &timestamp, &stripped)) {
		return -1;
	}
	if (timestamp == 0) {
		return SMB_VFS_NEXT_CHOWN(handle, fname, uid, gid);
	}
	conv = shadow_copy2_convert(talloc_tos(), handle, stripped, timestamp);
	TALLOC_FREE(stripped);
	if (conv == NULL) {
		return -1;
	}
	ret = SMB_VFS_NEXT_CHOWN(handle, conv, uid, gid);
	saved_errno = errno;
	TALLOC_FREE(conv);
	errno = saved_errno;
	return ret;
}