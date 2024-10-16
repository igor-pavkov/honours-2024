static int zr364xx_vidioc_streamon(struct file *file, void *priv,
				   enum v4l2_buf_type type)
{
	struct zr364xx_camera *cam = video_drvdata(file);
	int res;

	DBG("%s\n", __func__);

	if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	if (cam->owner && cam->owner != priv)
		return -EBUSY;

	res = zr364xx_prepare(cam);
	if (res)
		return res;
	res = videobuf_streamon(&cam->vb_vidq);
	if (res == 0) {
		zr364xx_start_acquire(cam);
		cam->owner = file->private_data;
	}
	return res;
}