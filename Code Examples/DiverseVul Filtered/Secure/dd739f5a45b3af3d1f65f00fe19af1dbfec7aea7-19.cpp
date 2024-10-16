static int __runMain(RMainCallback cb, const char *arg) {
	char *a = r_str_trim_dup (arg);
	int argc = 0;
	char **args = r_str_argv (a, &argc);
	int res = cb (argc, args);
	free (args);
	free (a);
	return res;
}