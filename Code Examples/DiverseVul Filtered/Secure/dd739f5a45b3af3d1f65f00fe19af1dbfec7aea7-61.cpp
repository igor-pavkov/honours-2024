static void tmpenvs_free(void *item) {
	r_sys_setenv (item, NULL);
	free (item);
}