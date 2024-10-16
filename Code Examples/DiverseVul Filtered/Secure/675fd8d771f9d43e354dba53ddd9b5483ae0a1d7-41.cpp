static int shadow_copy2_get_real_filename(struct vfs_handle_struct *handle,
					  const char *path,
					  const char *name,
					  TALLOC_CTX *mem_ctx,
					  char **found_name)
{
	time_t timestamp;
	char *stripped;
	ssize_t ret;
	int saved_errno;
	char *conv;

	DEBUG(10, ("shadow_copy2_get_real_filename called for path=[%s], "
		   "name=[%s]\n", path, name));

	if (!shadow_copy2_strip_snapshot(talloc_tos(), handle, path,
					 &timestamp, &stripped)) {
		DEBUG(10, ("shadow_copy2_strip_snapshot failed\n"));
		return -1;
	}
	if (timestamp == 0) {
		DEBUG(10, ("timestamp == 0\n"));
		return SMB_VFS_NEXT_GET_REAL_FILENAME(handle, path, name,
						      mem_ctx, found_name);
	}
	conv = shadow_copy2_convert(talloc_tos(), handle, stripped, timestamp);
	TALLOC_FREE(stripped);
	if (conv == NULL) {
		DEBUG(10, ("shadow_copy2_convert failed\n"));
		return -1;
	}
	DEBUG(10, ("Calling NEXT_GET_REAL_FILE_NAME for conv=[%s], "
		   "name=[%s]\n", conv, name));
	ret = SMB_VFS_NEXT_GET_REAL_FILENAME(handle, conv, name,
					     mem_ctx, found_name);
	DEBUG(10, ("NEXT_REAL_FILE_NAME returned %d\n", (int)ret));
	saved_errno = errno;
	TALLOC_FREE(conv);
	errno = saved_errno;
	return ret;
}