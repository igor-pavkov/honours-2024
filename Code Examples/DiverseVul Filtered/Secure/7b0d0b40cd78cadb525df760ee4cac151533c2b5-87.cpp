static int __init selinux_enabled_setup(char *str)
{
	unsigned long enabled;
	if (!kstrtoul(str, 0, &enabled))
		selinux_enabled = enabled ? 1 : 0;
	return 1;
}