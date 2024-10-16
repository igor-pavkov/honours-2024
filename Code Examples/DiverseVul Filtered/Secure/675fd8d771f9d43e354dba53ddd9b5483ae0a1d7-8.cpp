static char *shadow_copy2_insert_string(TALLOC_CTX *mem_ctx,
					struct vfs_handle_struct *handle,
					time_t snapshot)
{
	fstring snaptime_string;
	size_t snaptime_len = 0;
	char *result = NULL;
	struct shadow_copy2_config *config;

	SMB_VFS_HANDLE_GET_DATA(handle, config, struct shadow_copy2_config,
				return NULL);

	snaptime_len = shadow_copy2_posix_gmt_string(handle,
						     snapshot,
						     snaptime_string,
						     sizeof(snaptime_string));
	if (snaptime_len <= 0) {
		return NULL;
	}

	if (config->snapdir_absolute) {
		result = talloc_asprintf(mem_ctx, "%s/%s",
					 config->snapdir, snaptime_string);
	} else {
		result = talloc_asprintf(mem_ctx, "/%s/%s",
					 config->snapdir, snaptime_string);
	}
	if (result == NULL) {
		DEBUG(1, (__location__ " talloc_asprintf failed\n"));
	}

	return result;
}