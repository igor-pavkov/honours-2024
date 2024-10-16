int tomoyo_mount_permission(char *dev_name, struct path *path, char *type,
			    unsigned long flags, void *data_page)
{
	struct tomoyo_request_info r;
	int error;
	int idx;

	if (tomoyo_init_request_info(&r, NULL, TOMOYO_MAC_FILE_MOUNT)
	    == TOMOYO_CONFIG_DISABLED)
		return 0;
	if ((flags & MS_MGC_MSK) == MS_MGC_VAL)
		flags &= ~MS_MGC_MSK;
	if (flags & MS_REMOUNT) {
		type = TOMOYO_MOUNT_REMOUNT_KEYWORD;
		flags &= ~MS_REMOUNT;
	}
	if (flags & MS_MOVE) {
		type = TOMOYO_MOUNT_MOVE_KEYWORD;
		flags &= ~MS_MOVE;
	}
	if (flags & MS_BIND) {
		type = TOMOYO_MOUNT_BIND_KEYWORD;
		flags &= ~MS_BIND;
	}
	if (flags & MS_UNBINDABLE) {
		type = TOMOYO_MOUNT_MAKE_UNBINDABLE_KEYWORD;
		flags &= ~MS_UNBINDABLE;
	}
	if (flags & MS_PRIVATE) {
		type = TOMOYO_MOUNT_MAKE_PRIVATE_KEYWORD;
		flags &= ~MS_PRIVATE;
	}
	if (flags & MS_SLAVE) {
		type = TOMOYO_MOUNT_MAKE_SLAVE_KEYWORD;
		flags &= ~MS_SLAVE;
	}
	if (flags & MS_SHARED) {
		type = TOMOYO_MOUNT_MAKE_SHARED_KEYWORD;
		flags &= ~MS_SHARED;
	}
	if (!type)
		type = "<NULL>";
	idx = tomoyo_read_lock();
	error = tomoyo_mount_acl(&r, dev_name, path, type, flags);
	tomoyo_read_unlock(idx);
	return error;
}