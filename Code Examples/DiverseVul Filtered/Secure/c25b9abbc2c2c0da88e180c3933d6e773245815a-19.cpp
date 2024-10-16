static int __init skfd_init(void)
{
	return pci_register_driver(&skfddi_pci_driver);
}