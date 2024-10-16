int __init dm_interface_init(void)
{
	int r;

	r = dm_hash_init();
	if (r)
		return r;

	r = misc_register(&_dm_misc);
	if (r) {
		DMERR("misc_register failed for control device");
		dm_hash_exit();
		return r;
	}

	DMINFO("%d.%d.%d%s initialised: %s", DM_VERSION_MAJOR,
	       DM_VERSION_MINOR, DM_VERSION_PATCHLEVEL, DM_VERSION_EXTRA,
	       DM_DRIVER_EMAIL);
	return 0;
}