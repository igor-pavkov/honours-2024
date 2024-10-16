static void read_pipe_completion(struct urb *purb)
{
	struct zr364xx_pipeinfo *pipe_info;
	struct zr364xx_camera *cam;
	int pipe;

	pipe_info = purb->context;
	_DBG("%s %p, status %d\n", __func__, purb, purb->status);
	if (!pipe_info) {
		printk(KERN_ERR KBUILD_MODNAME ": no context!\n");
		return;
	}

	cam = pipe_info->cam;
	if (!cam) {
		printk(KERN_ERR KBUILD_MODNAME ": no context!\n");
		return;
	}

	/* if shutting down, do not resubmit, exit immediately */
	if (purb->status == -ESHUTDOWN) {
		DBG("%s, err shutdown\n", __func__);
		pipe_info->err_count++;
		return;
	}

	if (pipe_info->state == 0) {
		DBG("exiting USB pipe\n");
		return;
	}

	if (purb->actual_length > pipe_info->transfer_size) {
		dev_err(&cam->udev->dev, "wrong number of bytes\n");
		return;
	}

	if (purb->status == 0)
		zr364xx_read_video_callback(cam, pipe_info, purb);
	else {
		pipe_info->err_count++;
		DBG("%s: failed URB %d\n", __func__, purb->status);
	}

	pipe = usb_rcvbulkpipe(cam->udev, cam->read_endpoint);

	/* reuse urb */
	usb_fill_bulk_urb(pipe_info->stream_urb, cam->udev,
			  pipe,
			  pipe_info->transfer_buffer,
			  pipe_info->transfer_size,
			  read_pipe_completion, pipe_info);

	if (pipe_info->state != 0) {
		purb->status = usb_submit_urb(pipe_info->stream_urb,
					      GFP_ATOMIC);

		if (purb->status)
			dev_err(&cam->udev->dev,
				"error submitting urb (error=%i)\n",
				purb->status);
	} else
		DBG("read pipe complete state 0\n");
}