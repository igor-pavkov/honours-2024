static int vidioc_querybuf(struct file *file,
			    void *priv, struct v4l2_buffer *vb)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	struct usbvision_frame *frame;

	/* FIXME : must control
	   that buffers are mapped (VIDIOC_REQBUFS has been called) */
	if (vb->index >= usbvision->num_frames)
		return -EINVAL;
	/* Updating the corresponding frame state */
	vb->flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	frame = &usbvision->frame[vb->index];
	if (frame->grabstate >= frame_state_ready)
		vb->flags |= V4L2_BUF_FLAG_QUEUED;
	if (frame->grabstate >= frame_state_done)
		vb->flags |= V4L2_BUF_FLAG_DONE;
	if (frame->grabstate == frame_state_unused)
		vb->flags |= V4L2_BUF_FLAG_MAPPED;
	vb->memory = V4L2_MEMORY_MMAP;

	vb->m.offset = vb->index * PAGE_ALIGN(usbvision->max_frame_size);

	vb->memory = V4L2_MEMORY_MMAP;
	vb->field = V4L2_FIELD_NONE;
	vb->length = usbvision->curwidth *
		usbvision->curheight *
		usbvision->palette.bytes_per_pixel;
	vb->timestamp = usbvision->frame[vb->index].timestamp;
	vb->sequence = usbvision->frame[vb->index].sequence;
	return 0;
}