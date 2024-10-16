int __init ccid_initialize_builtins(void)
{
	int i, err = tfrc_lib_init();

	if (err)
		return err;

	for (i = 0; i < ARRAY_SIZE(ccids); i++) {
		err = ccid_activate(ccids[i]);
		if (err)
			goto unwind_registrations;
	}
	return 0;

unwind_registrations:
	while(--i >= 0)
		ccid_deactivate(ccids[i]);
	tfrc_lib_exit();
	return err;
}