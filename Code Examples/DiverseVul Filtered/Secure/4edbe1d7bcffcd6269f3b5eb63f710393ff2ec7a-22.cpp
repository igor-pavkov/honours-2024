static ioctl_fn lookup_ioctl(unsigned int cmd, int *ioctl_flags)
{
	static const struct {
		int cmd;
		int flags;
		ioctl_fn fn;
	} _ioctls[] = {
		{DM_VERSION_CMD, 0, NULL}, /* version is dealt with elsewhere */
		{DM_REMOVE_ALL_CMD, IOCTL_FLAGS_NO_PARAMS | IOCTL_FLAGS_ISSUE_GLOBAL_EVENT, remove_all},
		{DM_LIST_DEVICES_CMD, 0, list_devices},

		{DM_DEV_CREATE_CMD, IOCTL_FLAGS_NO_PARAMS | IOCTL_FLAGS_ISSUE_GLOBAL_EVENT, dev_create},
		{DM_DEV_REMOVE_CMD, IOCTL_FLAGS_NO_PARAMS | IOCTL_FLAGS_ISSUE_GLOBAL_EVENT, dev_remove},
		{DM_DEV_RENAME_CMD, IOCTL_FLAGS_ISSUE_GLOBAL_EVENT, dev_rename},
		{DM_DEV_SUSPEND_CMD, IOCTL_FLAGS_NO_PARAMS, dev_suspend},
		{DM_DEV_STATUS_CMD, IOCTL_FLAGS_NO_PARAMS, dev_status},
		{DM_DEV_WAIT_CMD, 0, dev_wait},

		{DM_TABLE_LOAD_CMD, 0, table_load},
		{DM_TABLE_CLEAR_CMD, IOCTL_FLAGS_NO_PARAMS, table_clear},
		{DM_TABLE_DEPS_CMD, 0, table_deps},
		{DM_TABLE_STATUS_CMD, 0, table_status},

		{DM_LIST_VERSIONS_CMD, 0, list_versions},

		{DM_TARGET_MSG_CMD, 0, target_message},
		{DM_DEV_SET_GEOMETRY_CMD, 0, dev_set_geometry},
		{DM_DEV_ARM_POLL, IOCTL_FLAGS_NO_PARAMS, dev_arm_poll},
		{DM_GET_TARGET_VERSION, 0, get_target_version},
	};

	if (unlikely(cmd >= ARRAY_SIZE(_ioctls)))
		return NULL;

	*ioctl_flags = _ioctls[cmd].flags;
	return _ioctls[cmd].fn;
}