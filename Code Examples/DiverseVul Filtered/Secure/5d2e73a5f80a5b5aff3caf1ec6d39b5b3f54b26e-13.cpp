static int zr364xx_vidioc_querybuf(struct file *file,
				void *priv,
				struct v4l2_buffer *p)
{
	int rc;
	struct zr364xx_camera *cam = video_drvdata(file);
	rc = videobuf_querybuf(&cam->vb_vidq, p);
	return rc;
}