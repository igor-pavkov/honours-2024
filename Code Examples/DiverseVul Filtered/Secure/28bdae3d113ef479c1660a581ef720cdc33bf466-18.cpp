static int find_mount_flag(const char *s, unsigned len, int *on, int *flag)
{
	int i;

	for (i = 0; mount_flags[i].opt != NULL; i++) {
		const char *opt = mount_flags[i].opt;
		if (strlen(opt) == len && strncmp(opt, s, len) == 0) {
			*on = mount_flags[i].on;
			*flag = mount_flags[i].flag;
			if (!mount_flags[i].safe && getuid() != 0) {
				*flag = 0;
				fprintf(stderr,
					"%s: unsafe option %s ignored\n",
					progname, opt);
			}
			return 1;
		}
	}
	return 0;
}