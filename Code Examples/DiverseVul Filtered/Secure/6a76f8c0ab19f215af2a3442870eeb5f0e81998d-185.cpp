void ftrace_bug(int failed, unsigned long ip)
{
	switch (failed) {
	case -EFAULT:
		FTRACE_WARN_ON_ONCE(1);
		pr_info("ftrace faulted on modifying ");
		print_ip_sym(ip);
		break;
	case -EINVAL:
		FTRACE_WARN_ON_ONCE(1);
		pr_info("ftrace failed to modify ");
		print_ip_sym(ip);
		print_ip_ins(" actual: ", (unsigned char *)ip);
		printk(KERN_CONT "\n");
		break;
	case -EPERM:
		FTRACE_WARN_ON_ONCE(1);
		pr_info("ftrace faulted on writing ");
		print_ip_sym(ip);
		break;
	default:
		FTRACE_WARN_ON_ONCE(1);
		pr_info("ftrace faulted on unknown error ");
		print_ip_sym(ip);
	}
}