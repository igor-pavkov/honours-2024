static void __exit sixpack_exit_driver(void)
{
	tty_unregister_ldisc(&sp_ldisc);
}