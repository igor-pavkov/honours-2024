static int vidioc_qbuf(struct file *file, void *priv, struct v4l2_buffer *vb)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	struct usbvision_frame *frame;
	unsigned long lock_flags;

	/* FIXME : works only on VIDEO_CAPTURE MODE, MMAP. */
	if (vb->index >= usbvision->num_frames)
		return -EINVAL;

	frame = &usbvision->frame[vb->index];

	if (frame->grabstate != frame_state_unused)
		return -EAGAIN;

	/* Mark it as ready and enqueue frame */
	frame->grabstate = frame_state_ready;
	frame->scanstate = scan_state_scanning;
	frame->scanlength = 0;	/* Accumulated in usbvision_parse_data() */

	vb->flags &= ~V4L2_BUF_FLAG_DONE;

	/* set v4l2_format index */
	frame->v4l2_format = usbvision->palette;

	spin_lock_irqsave(&usbvision->queue_lock, lock_flags);
	list_add_tail(&usbvision->frame[vb->index].frame, &usbvision->inqueue);
	spin_unlock_irqrestore(&usbvision->queue_lock, lock_flags);

	return 0;
}