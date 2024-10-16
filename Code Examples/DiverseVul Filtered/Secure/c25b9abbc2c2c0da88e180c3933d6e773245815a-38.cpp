static void __exit skfd_exit(void)
{
	pci_unregister_driver(&skfddi_pci_driver);
}