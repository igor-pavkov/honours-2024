static void zr364xx_release(struct v4l2_device *v4l2_dev)
{
	struct zr364xx_camera *cam =
		container_of(v4l2_dev, struct zr364xx_camera, v4l2_dev);
	unsigned long i;

	v4l2_device_unregister(&cam->v4l2_dev);

	videobuf_mmap_free(&cam->vb_vidq);

	/* release sys buffers */
	for (i = 0; i < FRAMES; i++) {
		if (cam->buffer.frame[i].lpvbits) {
			DBG("vfree %p\n", cam->buffer.frame[i].lpvbits);
			vfree(cam->buffer.frame[i].lpvbits);
		}
		cam->buffer.frame[i].lpvbits = NULL;
	}

	v4l2_ctrl_handler_free(&cam->ctrl_handler);
	/* release transfer buffer */
	kfree(cam->pipe->transfer_buffer);
	kfree(cam);
}