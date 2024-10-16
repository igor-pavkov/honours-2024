static void __exit mkiss_exit_driver(void)
{
	tty_unregister_ldisc(&ax_ldisc);
}