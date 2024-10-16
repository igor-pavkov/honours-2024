int lxc_switch_uid_gid(uid_t uid, gid_t gid)
{
	if (setgid(gid) < 0) {
		SYSERROR("Failed to switch to gid %d.", gid);
		return -errno;
	}
	NOTICE("Switched to gid %d.", gid);

	if (setuid(uid) < 0) {
		SYSERROR("Failed to switch to uid %d.", uid);
		return -errno;
	}
	NOTICE("Switched to uid %d.", uid);

	return 0;
}