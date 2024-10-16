static int vidioc_s_std(struct file *file, void *priv, v4l2_std_id id)
{
	struct usb_usbvision *usbvision = video_drvdata(file);

	usbvision->tvnorm_id = id;

	call_all(usbvision, video, s_std, usbvision->tvnorm_id);
	/* propagate the change to the decoder */
	usbvision_muxsel(usbvision, usbvision->ctl_input);

	return 0;
}