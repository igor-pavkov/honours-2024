static int __init checkreqprot_setup(char *str)
{
	unsigned long checkreqprot;

	if (!kstrtoul(str, 0, &checkreqprot)) {
		selinux_checkreqprot_boot = checkreqprot ? 1 : 0;
		if (checkreqprot)
			pr_warn("SELinux: checkreqprot set to 1 via kernel parameter.  This is deprecated and will be rejected in a future kernel release.\n");
	}
	return 1;
}