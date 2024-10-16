static int __init enforcing_setup(char *str)
{
	unsigned long enforcing;
	if (!kstrtoul(str, 0, &enforcing))
		selinux_enforcing = enforcing ? 1 : 0;
	return 1;
}