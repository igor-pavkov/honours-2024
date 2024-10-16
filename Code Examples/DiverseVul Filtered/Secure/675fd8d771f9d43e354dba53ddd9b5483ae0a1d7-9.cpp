static bool shadow_copy2_snapshot_to_gmt(vfs_handle_struct *handle,
					 const char *name,
					 char *gmt, size_t gmt_len)
{
	struct tm timestamp;
	time_t timestamp_t;
	unsigned long int timestamp_long;
	const char *fmt;
	struct shadow_copy2_config *config;

	SMB_VFS_HANDLE_GET_DATA(handle, config, struct shadow_copy2_config,
				return NULL);

	fmt = config->gmt_format;

	ZERO_STRUCT(timestamp);
	if (config->use_sscanf) {
		if (sscanf(name, fmt, &timestamp_long) != 1) {
			DEBUG(10, ("shadow_copy2_snapshot_to_gmt: "
				   "no sscanf match %s: %s\n",
				   fmt, name));
			return false;
		}
		timestamp_t = timestamp_long;
		gmtime_r(&timestamp_t, &timestamp);
	} else {
		if (strptime(name, fmt, &timestamp) == NULL) {
			DEBUG(10, ("shadow_copy2_snapshot_to_gmt: "
				   "no match %s: %s\n",
				   fmt, name));
			return false;
		}
		DEBUG(10, ("shadow_copy2_snapshot_to_gmt: match %s: %s\n",
			   fmt, name));
		
		if (config->use_localtime) {
			timestamp.tm_isdst = -1;
			timestamp_t = mktime(&timestamp);
			gmtime_r(&timestamp_t, &timestamp);
		}
	}

	strftime(gmt, gmt_len, GMT_FORMAT, &timestamp);
	return true;
}