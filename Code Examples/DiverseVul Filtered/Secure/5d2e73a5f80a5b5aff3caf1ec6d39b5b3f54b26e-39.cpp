static int zr364xx_vidioc_g_fmt_vid_cap(struct file *file, void *priv,
				    struct v4l2_format *f)
{
	struct zr364xx_camera *cam;

	if (!file)
		return -ENODEV;
	cam = video_drvdata(file);

	f->fmt.pix.pixelformat = formats[0].fourcc;
	f->fmt.pix.field = V4L2_FIELD_NONE;
	f->fmt.pix.width = cam->width;
	f->fmt.pix.height = cam->height;
	f->fmt.pix.bytesperline = f->fmt.pix.width * 2;
	f->fmt.pix.sizeimage = f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace = V4L2_COLORSPACE_JPEG;
	return 0;
}