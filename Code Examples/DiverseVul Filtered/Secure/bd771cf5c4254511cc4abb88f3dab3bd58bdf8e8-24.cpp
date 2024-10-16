static void __exit sm712fb_exit(void)
{
	pci_unregister_driver(&smtcfb_driver);
}