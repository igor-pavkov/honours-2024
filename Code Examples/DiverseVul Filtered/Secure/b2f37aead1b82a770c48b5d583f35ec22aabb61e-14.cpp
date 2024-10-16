static int __init mkiss_init_driver(void)
{
	int status;

	printk(banner);

	status = tty_register_ldisc(&ax_ldisc);
	if (status != 0)
		printk(msg_regfail, status);

	return status;
}