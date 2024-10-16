static int zr364xx_start_readpipe(struct zr364xx_camera *cam)
{
	int pipe;
	int retval;
	struct zr364xx_pipeinfo *pipe_info = cam->pipe;
	pipe = usb_rcvbulkpipe(cam->udev, cam->read_endpoint);
	DBG("%s: start pipe IN x%x\n", __func__, cam->read_endpoint);

	pipe_info->state = 1;
	pipe_info->err_count = 0;
	pipe_info->stream_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!pipe_info->stream_urb)
		return -ENOMEM;
	/* transfer buffer allocated in board_init */
	usb_fill_bulk_urb(pipe_info->stream_urb, cam->udev,
			  pipe,
			  pipe_info->transfer_buffer,
			  pipe_info->transfer_size,
			  read_pipe_completion, pipe_info);

	DBG("submitting URB %p\n", pipe_info->stream_urb);
	retval = usb_submit_urb(pipe_info->stream_urb, GFP_KERNEL);
	if (retval) {
		printk(KERN_ERR KBUILD_MODNAME ": start read pipe failed\n");
		return retval;
	}

	return 0;
}