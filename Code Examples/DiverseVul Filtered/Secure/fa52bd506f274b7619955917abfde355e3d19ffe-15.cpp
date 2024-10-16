static int vidioc_streamoff(struct file *file,
			    void *priv, enum v4l2_buf_type type)
{
	struct usb_usbvision *usbvision = video_drvdata(file);

	if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	if (usbvision->streaming == stream_on) {
		usbvision_stream_interrupt(usbvision);
		/* Stop all video streamings */
		call_all(usbvision, video, s_stream, 0);
	}
	usbvision_empty_framequeues(usbvision);

	return 0;
}