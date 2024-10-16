static int vidioc_s_fmt_vid_cap(struct file *file, void *priv,
			       struct v4l2_format *vf)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	int ret;

	ret = vidioc_try_fmt_vid_cap(file, priv, vf);
	if (ret)
		return ret;

	/* stop io in case it is already in progress */
	if (usbvision->streaming == stream_on) {
		ret = usbvision_stream_interrupt(usbvision);
		if (ret)
			return ret;
	}
	usbvision_frames_free(usbvision);
	usbvision_empty_framequeues(usbvision);

	usbvision->cur_frame = NULL;

	/* by now we are committed to the new data... */
	usbvision_set_output(usbvision, vf->fmt.pix.width, vf->fmt.pix.height);

	return 0;
}