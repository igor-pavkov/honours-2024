static int __init wanxl_init_module(void)
{
#ifdef MODULE
	pr_info("%s\n", version);
#endif
	return pci_register_driver(&wanxl_pci_driver);
}