static int zr364xx_board_init(struct zr364xx_camera *cam)
{
	struct zr364xx_pipeinfo *pipe = cam->pipe;
	unsigned long i;

	DBG("board init: %p\n", cam);
	memset(pipe, 0, sizeof(*pipe));
	pipe->cam = cam;
	pipe->transfer_size = BUFFER_SIZE;

	pipe->transfer_buffer = kzalloc(pipe->transfer_size,
					GFP_KERNEL);
	if (!pipe->transfer_buffer) {
		DBG("out of memory!\n");
		return -ENOMEM;
	}

	cam->b_acquire = 0;
	cam->frame_count = 0;

	/*** start create system buffers ***/
	for (i = 0; i < FRAMES; i++) {
		/* always allocate maximum size for system buffers */
		cam->buffer.frame[i].lpvbits = vmalloc(MAX_FRAME_SIZE);

		DBG("valloc %p, idx %lu, pdata %p\n",
			&cam->buffer.frame[i], i,
			cam->buffer.frame[i].lpvbits);
		if (!cam->buffer.frame[i].lpvbits) {
			printk(KERN_INFO KBUILD_MODNAME ": out of memory. Using less frames\n");
			break;
		}
	}

	if (i == 0) {
		printk(KERN_INFO KBUILD_MODNAME ": out of memory. Aborting\n");
		kfree(cam->pipe->transfer_buffer);
		cam->pipe->transfer_buffer = NULL;
		return -ENOMEM;
	} else
		cam->buffer.dwFrames = i;

	/* make sure internal states are set */
	for (i = 0; i < FRAMES; i++) {
		cam->buffer.frame[i].ulState = ZR364XX_READ_IDLE;
		cam->buffer.frame[i].cur_size = 0;
	}

	cam->cur_frame = 0;
	cam->last_frame = -1;
	/*** end create system buffers ***/

	/* start read pipe */
	zr364xx_start_readpipe(cam);
	DBG(": board initialized\n");
	return 0;
}