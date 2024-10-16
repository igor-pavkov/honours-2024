int cpia2_usb_init(void)
{
	return usb_register(&cpia2_driver);
}