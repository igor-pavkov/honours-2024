static int zr364xx_vidioc_try_fmt_vid_cap(struct file *file, void *priv,
				      struct v4l2_format *f)
{
	struct zr364xx_camera *cam = video_drvdata(file);
	char pixelformat_name[5];

	if (!cam)
		return -ENODEV;

	if (f->fmt.pix.pixelformat != V4L2_PIX_FMT_JPEG) {
		DBG("%s: unsupported pixelformat V4L2_PIX_FMT_%s\n", __func__,
		    decode_fourcc(f->fmt.pix.pixelformat, pixelformat_name));
		return -EINVAL;
	}

	if (!(f->fmt.pix.width == 160 && f->fmt.pix.height == 120) &&
	    !(f->fmt.pix.width == 640 && f->fmt.pix.height == 480)) {
		f->fmt.pix.width = 320;
		f->fmt.pix.height = 240;
	}

	f->fmt.pix.field = V4L2_FIELD_NONE;
	f->fmt.pix.bytesperline = f->fmt.pix.width * 2;
	f->fmt.pix.sizeimage = f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace = V4L2_COLORSPACE_JPEG;
	DBG("%s: V4L2_PIX_FMT_%s (%d) ok!\n", __func__,
	    decode_fourcc(f->fmt.pix.pixelformat, pixelformat_name),
	    f->fmt.pix.field);
	return 0;
}