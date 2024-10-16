static int __init sm712fb_init(void)
{
	char *option = NULL;

	if (fb_get_options("sm712fb", &option))
		return -ENODEV;
	if (option && *option)
		mode_option = option;
	sm7xx_vga_setup(mode_option);

	return pci_register_driver(&smtcfb_driver);
}