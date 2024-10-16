int lxc_setgroups(int size, gid_t list[])
{
	if (setgroups(size, list) < 0) {
		SYSERROR("Failed to setgroups().");
		return -errno;
	}
	NOTICE("Dropped additional groups.");

	return 0;
}