static void __exit usbvision_exit(void)
{
	PDEBUG(DBG_PROBE, "");

	usb_deregister(&usbvision_driver);
	PDEBUG(DBG_PROBE, "success");
}