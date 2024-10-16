static int vidioc_g_std(struct file *file, void *priv, v4l2_std_id *id)
{
	struct usb_usbvision *usbvision = video_drvdata(file);

	*id = usbvision->tvnorm_id;
	return 0;
}