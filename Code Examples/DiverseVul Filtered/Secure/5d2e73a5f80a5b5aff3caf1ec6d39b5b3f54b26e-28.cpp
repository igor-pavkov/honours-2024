static int zr364xx_open(struct file *file)
{
	struct zr364xx_camera *cam = video_drvdata(file);
	int err;

	DBG("%s\n", __func__);

	if (mutex_lock_interruptible(&cam->lock))
		return -ERESTARTSYS;

	err = v4l2_fh_open(file);
	if (err)
		goto out;

	/* Added some delay here, since opening/closing the camera quickly,
	 * like Ekiga does during its startup, can crash the webcam
	 */
	mdelay(100);
	err = 0;

out:
	mutex_unlock(&cam->lock);
	DBG("%s: %d\n", __func__, err);
	return err;
}