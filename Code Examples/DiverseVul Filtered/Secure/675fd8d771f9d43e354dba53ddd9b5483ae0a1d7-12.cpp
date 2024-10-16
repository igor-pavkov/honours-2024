static size_t shadow_copy2_posix_gmt_string(struct vfs_handle_struct *handle,
					    time_t snapshot,
					    char *snaptime_string,
					    size_t len)
{
	struct tm snap_tm;
	size_t snaptime_len;
	struct shadow_copy2_config *config;

	SMB_VFS_HANDLE_GET_DATA(handle, config, struct shadow_copy2_config,
				return 0);

	if (config->use_sscanf) {
		snaptime_len = snprintf(snaptime_string,
					len,
					config->gmt_format,
					(unsigned long)snapshot);
		if (snaptime_len <= 0) {
			DEBUG(10, ("snprintf failed\n"));
			return snaptime_len;
		}
	} else {
		if (config->use_localtime) {
			if (localtime_r(&snapshot, &snap_tm) == 0) {
				DEBUG(10, ("gmtime_r failed\n"));
				return -1;
			}
		} else {
			if (gmtime_r(&snapshot, &snap_tm) == 0) {
				DEBUG(10, ("gmtime_r failed\n"));
				return -1;
			}
		}
		snaptime_len = strftime(snaptime_string,
					len,
					config->gmt_format,
					&snap_tm);
		if (snaptime_len == 0) {
			DEBUG(10, ("strftime failed\n"));
			return 0;
		}
	}

	return snaptime_len;
}