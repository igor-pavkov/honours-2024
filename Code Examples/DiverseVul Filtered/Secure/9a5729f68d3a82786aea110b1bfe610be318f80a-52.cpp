static void __exit usb_sisusb_exit(void)
{
	usb_deregister(&sisusb_driver);
}