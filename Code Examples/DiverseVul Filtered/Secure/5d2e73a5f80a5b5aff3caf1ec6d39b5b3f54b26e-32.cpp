static void zr364xx_stop_readpipe(struct zr364xx_camera *cam)
{
	struct zr364xx_pipeinfo *pipe_info;

	if (!cam) {
		printk(KERN_ERR KBUILD_MODNAME ": invalid device\n");
		return;
	}
	DBG("stop read pipe\n");
	pipe_info = cam->pipe;
	if (pipe_info) {
		if (pipe_info->state != 0)
			pipe_info->state = 0;

		if (pipe_info->stream_urb) {
			/* cancel urb */
			usb_kill_urb(pipe_info->stream_urb);
			usb_free_urb(pipe_info->stream_urb);
			pipe_info->stream_urb = NULL;
		}
	}
	return;
}