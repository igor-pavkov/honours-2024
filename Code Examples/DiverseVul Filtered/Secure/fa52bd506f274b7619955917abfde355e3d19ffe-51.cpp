static int vidioc_g_register(struct file *file, void *priv,
				struct v4l2_dbg_register *reg)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	int err_code;

	/* NT100x has a 8-bit register space */
	err_code = usbvision_read_reg(usbvision, reg->reg&0xff);
	if (err_code < 0) {
		dev_err(&usbvision->vdev.dev,
			"%s: VIDIOC_DBG_G_REGISTER failed: error %d\n",
				__func__, err_code);
		return err_code;
	}
	reg->val = err_code;
	reg->size = 1;
	return 0;
}