static void __exit mod_exit(void)
{
	unregister_hdlc_protocol(&proto);
}