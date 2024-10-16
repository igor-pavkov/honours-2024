static int zr364xx_close(struct file *file)
{
	struct zr364xx_camera *cam;
	struct usb_device *udev;
	int i;

	DBG("%s\n", __func__);
	cam = video_drvdata(file);

	mutex_lock(&cam->lock);
	udev = cam->udev;

	if (file->private_data == cam->owner) {
		/* turn off stream */
		if (cam->b_acquire)
			zr364xx_stop_acquire(cam);
		videobuf_streamoff(&cam->vb_vidq);

		for (i = 0; i < 2; i++) {
			send_control_msg(udev, 1, init[cam->method][i].value,
					0, init[cam->method][i].bytes,
					init[cam->method][i].size);
		}
		cam->owner = NULL;
	}

	/* Added some delay here, since opening/closing the camera quickly,
	 * like Ekiga does during its startup, can crash the webcam
	 */
	mdelay(100);
	mutex_unlock(&cam->lock);
	return v4l2_fh_release(file);
}