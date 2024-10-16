void ccid_cleanup_builtins(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ccids); i++)
		ccid_deactivate(ccids[i]);
	tfrc_lib_exit();
}