static int vidioc_s_register(struct file *file, void *priv,
				const struct v4l2_dbg_register *reg)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	int err_code;

	/* NT100x has a 8-bit register space */
	err_code = usbvision_write_reg(usbvision, reg->reg & 0xff, reg->val);
	if (err_code < 0) {
		dev_err(&usbvision->vdev.dev,
			"%s: VIDIOC_DBG_S_REGISTER failed: error %d\n",
				__func__, err_code);
		return err_code;
	}
	return 0;
}