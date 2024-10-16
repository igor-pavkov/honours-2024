static int zr364xx_vidioc_reqbufs(struct file *file, void *priv,
			  struct v4l2_requestbuffers *p)
{
	struct zr364xx_camera *cam = video_drvdata(file);

	if (cam->owner && cam->owner != priv)
		return -EBUSY;
	return videobuf_reqbufs(&cam->vb_vidq, p);
}