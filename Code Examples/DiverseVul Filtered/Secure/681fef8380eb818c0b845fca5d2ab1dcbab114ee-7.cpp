int __init usb_devio_init(void)
{
	int retval;

	retval = register_chrdev_region(USB_DEVICE_DEV, USB_DEVICE_MAX,
					"usb_device");
	if (retval) {
		printk(KERN_ERR "Unable to register minors for usb_device\n");
		goto out;
	}
	cdev_init(&usb_device_cdev, &usbdev_file_operations);
	retval = cdev_add(&usb_device_cdev, USB_DEVICE_DEV, USB_DEVICE_MAX);
	if (retval) {
		printk(KERN_ERR "Unable to get usb_device major %d\n",
		       USB_DEVICE_MAJOR);
		goto error_cdev;
	}
	usb_register_notify(&usbdev_nb);
out:
	return retval;

error_cdev:
	unregister_chrdev_region(USB_DEVICE_DEV, USB_DEVICE_MAX);
	goto out;
}