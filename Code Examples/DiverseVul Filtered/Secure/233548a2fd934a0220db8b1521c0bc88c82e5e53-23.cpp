static int __init uvc_init(void)
{
	int result;

	INIT_LIST_HEAD(&uvc_driver.devices);
	INIT_LIST_HEAD(&uvc_driver.controls);
	mutex_init(&uvc_driver.open_mutex);
	mutex_init(&uvc_driver.ctrl_mutex);

	uvc_ctrl_init();

	result = usb_register(&uvc_driver.driver);
	if (result == 0)
		printk(KERN_INFO DRIVER_DESC " (" DRIVER_VERSION ")\n");
	return result;
}