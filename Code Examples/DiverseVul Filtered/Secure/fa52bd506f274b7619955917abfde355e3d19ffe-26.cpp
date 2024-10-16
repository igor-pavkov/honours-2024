static int vidioc_streamon(struct file *file, void *priv, enum v4l2_buf_type i)
{
	struct usb_usbvision *usbvision = video_drvdata(file);

	usbvision->streaming = stream_on;
	call_all(usbvision, video, s_stream, 1);

	return 0;
}