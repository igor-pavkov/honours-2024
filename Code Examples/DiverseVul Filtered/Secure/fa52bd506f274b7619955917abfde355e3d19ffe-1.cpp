static int __init usbvision_init(void)
{
	int err_code;

	PDEBUG(DBG_PROBE, "");

	PDEBUG(DBG_IO,  "IO      debugging is enabled [video]");
	PDEBUG(DBG_PROBE, "PROBE   debugging is enabled [video]");
	PDEBUG(DBG_MMAP, "MMAP    debugging is enabled [video]");

	/* disable planar mode support unless compression enabled */
	if (isoc_mode != ISOC_MODE_COMPRESS) {
		/* FIXME : not the right way to set supported flag */
		usbvision_v4l2_format[6].supported = 0; /* V4L2_PIX_FMT_YVU420 */
		usbvision_v4l2_format[7].supported = 0; /* V4L2_PIX_FMT_YUV422P */
	}

	err_code = usb_register(&usbvision_driver);

	if (err_code == 0) {
		printk(KERN_INFO DRIVER_DESC " : " USBVISION_VERSION_STRING "\n");
		PDEBUG(DBG_PROBE, "success");
	}
	return err_code;
}