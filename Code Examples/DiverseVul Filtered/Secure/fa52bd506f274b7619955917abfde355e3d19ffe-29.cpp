static int vidioc_g_input(struct file *file, void *priv, unsigned int *input)
{
	struct usb_usbvision *usbvision = video_drvdata(file);

	*input = usbvision->ctl_input;
	return 0;
}