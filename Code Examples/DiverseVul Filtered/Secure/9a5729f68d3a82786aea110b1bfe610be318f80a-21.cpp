static int __init usb_sisusb_init(void)
{

#ifdef CONFIG_USB_SISUSBVGA_CON
	sisusb_init_concode();
#endif

	return usb_register(&sisusb_driver);
}