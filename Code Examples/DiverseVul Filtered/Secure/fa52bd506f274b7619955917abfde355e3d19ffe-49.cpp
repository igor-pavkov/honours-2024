static int vidioc_s_input(struct file *file, void *priv, unsigned int input)
{
	struct usb_usbvision *usbvision = video_drvdata(file);

	if (input >= usbvision->video_inputs)
		return -EINVAL;

	usbvision_muxsel(usbvision, input);
	usbvision_set_input(usbvision);
	usbvision_set_output(usbvision,
			     usbvision->curwidth,
			     usbvision->curheight);
	return 0;
}