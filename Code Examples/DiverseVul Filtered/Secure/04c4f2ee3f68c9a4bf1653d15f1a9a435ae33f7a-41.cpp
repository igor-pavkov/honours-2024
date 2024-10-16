static int vmentry_l1d_flush_parse(const char *s)
{
	unsigned int i;

	if (s) {
		for (i = 0; i < ARRAY_SIZE(vmentry_l1d_param); i++) {
			if (vmentry_l1d_param[i].for_parse &&
			    sysfs_streq(s, vmentry_l1d_param[i].option))
				return i;
		}
	}
	return -EINVAL;
}