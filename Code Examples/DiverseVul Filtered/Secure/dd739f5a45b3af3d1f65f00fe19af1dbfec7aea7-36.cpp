static int callback_foreach_kv(void *user, const char *k, const char *v) {
	r_cons_printf ("%s=%s\n", k, v);
	return 1;
}