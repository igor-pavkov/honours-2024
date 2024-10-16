static int vidioc_reqbufs(struct file *file,
			   void *priv, struct v4l2_requestbuffers *vr)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	int ret;

	RESTRICT_TO_RANGE(vr->count, 1, USBVISION_NUMFRAMES);

	/* Check input validity:
	   the user must do a VIDEO CAPTURE and MMAP method. */
	if (vr->memory != V4L2_MEMORY_MMAP)
		return -EINVAL;

	if (usbvision->streaming == stream_on) {
		ret = usbvision_stream_interrupt(usbvision);
		if (ret)
			return ret;
	}

	usbvision_frames_free(usbvision);
	usbvision_empty_framequeues(usbvision);
	vr->count = usbvision_frames_alloc(usbvision, vr->count);

	usbvision->cur_frame = NULL;

	return 0;
}