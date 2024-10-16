void cpia2_usb_cleanup(void)
{
	schedule_timeout(2 * HZ);
	usb_deregister(&cpia2_driver);
}