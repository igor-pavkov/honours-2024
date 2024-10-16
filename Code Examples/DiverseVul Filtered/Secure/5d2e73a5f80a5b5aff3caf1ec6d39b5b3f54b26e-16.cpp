static int zr364xx_vidioc_qbuf(struct file *file,
				void *priv,
				struct v4l2_buffer *p)
{
	int rc;
	struct zr364xx_camera *cam = video_drvdata(file);
	_DBG("%s\n", __func__);
	if (cam->owner && cam->owner != priv)
		return -EBUSY;
	rc = videobuf_qbuf(&cam->vb_vidq, p);
	return rc;
}