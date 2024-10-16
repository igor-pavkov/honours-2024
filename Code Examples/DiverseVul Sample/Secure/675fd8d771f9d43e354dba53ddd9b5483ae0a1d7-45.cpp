static char *shadow_copy2_realpath(vfs_handle_struct *handle,
				   const char *fname)
{
	time_t timestamp;
	char *stripped = NULL;
	char *tmp = NULL;
	char *result = NULL;
	char *inserted = NULL;
	char *inserted_to, *inserted_end;
	int saved_errno;

	if (!shadow_copy2_strip_snapshot(talloc_tos(), handle, fname,
					 &timestamp, &stripped)) {
		goto done;
	}
	if (timestamp == 0) {
		return SMB_VFS_NEXT_REALPATH(handle, fname);
	}

	tmp = shadow_copy2_convert(talloc_tos(), handle, stripped, timestamp);
	if (tmp == NULL) {
		goto done;
	}

	result = SMB_VFS_NEXT_REALPATH(handle, tmp);
	if (result == NULL) {
		goto done;
	}

	/*
	 * Take away what we've inserted. This removes the @GMT-thingy
	 * completely, but will give a path under the share root.
	 */
	inserted = shadow_copy2_insert_string(talloc_tos(), handle, timestamp);
	if (inserted == NULL) {
		goto done;
	}
	inserted_to = strstr_m(result, inserted);
	if (inserted_to == NULL) {
		DEBUG(2, ("SMB_VFS_NEXT_REALPATH removed %s\n", inserted));
		goto done;
	}
	inserted_end = inserted_to + talloc_get_size(inserted) - 1;
	memmove(inserted_to, inserted_end, strlen(inserted_end)+1);

done:
	saved_errno = errno;
	TALLOC_FREE(inserted);
	TALLOC_FREE(tmp);
	TALLOC_FREE(stripped);
	errno = saved_errno;
	return result;
}