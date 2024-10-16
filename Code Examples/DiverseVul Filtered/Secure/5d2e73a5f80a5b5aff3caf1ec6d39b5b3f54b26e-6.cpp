static ssize_t zr364xx_read(struct file *file, char __user *buf, size_t count,
			    loff_t * ppos)
{
	struct zr364xx_camera *cam = video_drvdata(file);
	int err = 0;

	_DBG("%s\n", __func__);

	if (!buf)
		return -EINVAL;

	if (!count)
		return -EINVAL;

	if (mutex_lock_interruptible(&cam->lock))
		return -ERESTARTSYS;

	err = zr364xx_vidioc_streamon(file, file->private_data,
				V4L2_BUF_TYPE_VIDEO_CAPTURE);
	if (err == 0) {
		DBG("%s: reading %d bytes at pos %d.\n", __func__,
				(int) count, (int) *ppos);

		/* NoMan Sux ! */
		err = videobuf_read_one(&cam->vb_vidq, buf, count, ppos,
					file->f_flags & O_NONBLOCK);
	}
	mutex_unlock(&cam->lock);
	return err;
}