static int zr364xx_vidioc_streamoff(struct file *file, void *priv,
				    enum v4l2_buf_type type)
{
	struct zr364xx_camera *cam = video_drvdata(file);

	DBG("%s\n", __func__);
	if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	if (cam->owner && cam->owner != priv)
		return -EBUSY;
	zr364xx_stop_acquire(cam);
	return videobuf_streamoff(&cam->vb_vidq);
}