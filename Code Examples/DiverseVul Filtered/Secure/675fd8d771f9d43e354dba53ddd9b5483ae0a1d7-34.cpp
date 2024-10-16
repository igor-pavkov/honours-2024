static uint64_t shadow_copy2_disk_free(vfs_handle_struct *handle,
				       const char *path, uint64_t *bsize,
				       uint64_t *dfree, uint64_t *dsize)
{
	time_t timestamp;
	char *stripped;
	ssize_t ret;
	int saved_errno;
	char *conv;

	if (!shadow_copy2_strip_snapshot(talloc_tos(), handle, path,
					 &timestamp, &stripped)) {
		return -1;
	}
	if (timestamp == 0) {
		return SMB_VFS_NEXT_DISK_FREE(handle, path,
					      bsize, dfree, dsize);
	}

	conv = shadow_copy2_convert(talloc_tos(), handle, stripped, timestamp);
	TALLOC_FREE(stripped);
	if (conv == NULL) {
		return -1;
	}

	ret = SMB_VFS_NEXT_DISK_FREE(handle, conv, bsize, dfree, dsize);

	saved_errno = errno;
	TALLOC_FREE(conv);
	errno = saved_errno;

	return ret;
}