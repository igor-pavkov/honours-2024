void rfcomm_cleanup_ttys(void)
{
	tty_unregister_driver(rfcomm_tty_driver);
	put_tty_driver(rfcomm_tty_driver);
}