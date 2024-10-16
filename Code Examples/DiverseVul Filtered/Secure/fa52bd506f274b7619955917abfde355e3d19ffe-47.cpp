static ssize_t usbvision_read(struct file *file, char __user *buf,
		      size_t count, loff_t *ppos)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	int noblock = file->f_flags & O_NONBLOCK;
	unsigned long lock_flags;
	int ret, i;
	struct usbvision_frame *frame;

	PDEBUG(DBG_IO, "%s: %ld bytes, noblock=%d", __func__,
	       (unsigned long)count, noblock);

	if (!USBVISION_IS_OPERATIONAL(usbvision) || (buf == NULL))
		return -EFAULT;

	/* This entry point is compatible with the mmap routines
	   so that a user can do either VIDIOC_QBUF/VIDIOC_DQBUF
	   to get frames or call read on the device. */
	if (!usbvision->num_frames) {
		/* First, allocate some frames to work with
		   if this has not been done with VIDIOC_REQBUF */
		usbvision_frames_free(usbvision);
		usbvision_empty_framequeues(usbvision);
		usbvision_frames_alloc(usbvision, USBVISION_NUMFRAMES);
	}

	if (usbvision->streaming != stream_on) {
		/* no stream is running, make it running ! */
		usbvision->streaming = stream_on;
		call_all(usbvision, video, s_stream, 1);
	}

	/* Then, enqueue as many frames as possible
	   (like a user of VIDIOC_QBUF would do) */
	for (i = 0; i < usbvision->num_frames; i++) {
		frame = &usbvision->frame[i];
		if (frame->grabstate == frame_state_unused) {
			/* Mark it as ready and enqueue frame */
			frame->grabstate = frame_state_ready;
			frame->scanstate = scan_state_scanning;
			/* Accumulated in usbvision_parse_data() */
			frame->scanlength = 0;

			/* set v4l2_format index */
			frame->v4l2_format = usbvision->palette;

			spin_lock_irqsave(&usbvision->queue_lock, lock_flags);
			list_add_tail(&frame->frame, &usbvision->inqueue);
			spin_unlock_irqrestore(&usbvision->queue_lock,
					       lock_flags);
		}
	}

	/* Then try to steal a frame (like a VIDIOC_DQBUF would do) */
	if (list_empty(&(usbvision->outqueue))) {
		if (noblock)
			return -EAGAIN;

		ret = wait_event_interruptible
			(usbvision->wait_frame,
			 !list_empty(&(usbvision->outqueue)));
		if (ret)
			return ret;
	}

	spin_lock_irqsave(&usbvision->queue_lock, lock_flags);
	frame = list_entry(usbvision->outqueue.next,
			   struct usbvision_frame, frame);
	list_del(usbvision->outqueue.next);
	spin_unlock_irqrestore(&usbvision->queue_lock, lock_flags);

	/* An error returns an empty frame */
	if (frame->grabstate == frame_state_error) {
		frame->bytes_read = 0;
		return 0;
	}

	PDEBUG(DBG_IO, "%s: frmx=%d, bytes_read=%ld, scanlength=%ld",
	       __func__,
	       frame->index, frame->bytes_read, frame->scanlength);

	/* copy bytes to user space; we allow for partials reads */
	if ((count + frame->bytes_read) > (unsigned long)frame->scanlength)
		count = frame->scanlength - frame->bytes_read;

	if (copy_to_user(buf, frame->data + frame->bytes_read, count))
		return -EFAULT;

	frame->bytes_read += count;
	PDEBUG(DBG_IO, "%s: {copy} count used=%ld, new bytes_read=%ld",
	       __func__,
	       (unsigned long)count, frame->bytes_read);

#if 1
	/*
	 * FIXME:
	 * For now, forget the frame if it has not been read in one shot.
	 */
	frame->bytes_read = 0;

	/* Mark it as available to be used again. */
	frame->grabstate = frame_state_unused;
#else
	if (frame->bytes_read >= frame->scanlength) {
		/* All data has been read */
		frame->bytes_read = 0;

		/* Mark it as available to be used again. */
		frame->grabstate = frame_state_unused;
	}
#endif

	return count;
}