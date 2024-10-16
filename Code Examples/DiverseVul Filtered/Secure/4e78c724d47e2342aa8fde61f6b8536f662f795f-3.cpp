static int tomoyo_audit_mount_log(struct tomoyo_request_info *r)
{
	const char *dev = r->param.mount.dev->name;
	const char *dir = r->param.mount.dir->name;
	const char *type = r->param.mount.type->name;
	const unsigned long flags = r->param.mount.flags;
	if (r->granted)
		return 0;
	if (!strcmp(type, TOMOYO_MOUNT_REMOUNT_KEYWORD))
		tomoyo_warn_log(r, "mount -o remount %s 0x%lX", dir, flags);
	else if (!strcmp(type, TOMOYO_MOUNT_BIND_KEYWORD)
		 || !strcmp(type, TOMOYO_MOUNT_MOVE_KEYWORD))
		tomoyo_warn_log(r, "mount %s %s %s 0x%lX", type, dev, dir,
				flags);
	else if (!strcmp(type, TOMOYO_MOUNT_MAKE_UNBINDABLE_KEYWORD) ||
		 !strcmp(type, TOMOYO_MOUNT_MAKE_PRIVATE_KEYWORD) ||
		 !strcmp(type, TOMOYO_MOUNT_MAKE_SLAVE_KEYWORD) ||
		 !strcmp(type, TOMOYO_MOUNT_MAKE_SHARED_KEYWORD))
		tomoyo_warn_log(r, "mount %s %s 0x%lX", type, dir, flags);
	else
		tomoyo_warn_log(r, "mount -t %s %s %s 0x%lX", type, dev, dir,
				flags);
	return tomoyo_supervisor(r,
				 TOMOYO_KEYWORD_ALLOW_MOUNT "%s %s %s 0x%lX\n",
				 tomoyo_pattern(r->param.mount.dev),
				 tomoyo_pattern(r->param.mount.dir), type,
				 flags);
}