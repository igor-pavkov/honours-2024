static int vidioc_dqbuf(struct file *file, void *priv, struct v4l2_buffer *vb)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	int ret;
	struct usbvision_frame *f;
	unsigned long lock_flags;

	if (list_empty(&(usbvision->outqueue))) {
		if (usbvision->streaming == stream_idle)
			return -EINVAL;
		ret = wait_event_interruptible
			(usbvision->wait_frame,
			 !list_empty(&(usbvision->outqueue)));
		if (ret)
			return ret;
	}

	spin_lock_irqsave(&usbvision->queue_lock, lock_flags);
	f = list_entry(usbvision->outqueue.next,
		       struct usbvision_frame, frame);
	list_del(usbvision->outqueue.next);
	spin_unlock_irqrestore(&usbvision->queue_lock, lock_flags);

	f->grabstate = frame_state_unused;

	vb->memory = V4L2_MEMORY_MMAP;
	vb->flags = V4L2_BUF_FLAG_MAPPED |
		V4L2_BUF_FLAG_QUEUED |
		V4L2_BUF_FLAG_DONE |
		V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	vb->index = f->index;
	vb->sequence = f->sequence;
	vb->timestamp = f->timestamp;
	vb->field = V4L2_FIELD_NONE;
	vb->bytesused = f->scanlength;

	return 0;
}