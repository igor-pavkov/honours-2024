static int cmd_pipein(void *user, const char *input) {
	char *buf = strdup (input);
	int len = r_str_unescape (buf);
	r_cons_readpush (buf, len);
	free (buf);
	return 0;
}