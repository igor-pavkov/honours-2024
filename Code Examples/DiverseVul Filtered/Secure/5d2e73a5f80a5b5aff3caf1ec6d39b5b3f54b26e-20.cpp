static int zr364xx_vidioc_enum_fmt_vid_cap(struct file *file,
				       void *priv, struct v4l2_fmtdesc *f)
{
	if (f->index > 0)
		return -EINVAL;
	f->flags = V4L2_FMT_FLAG_COMPRESSED;
	strscpy(f->description, formats[0].name, sizeof(f->description));
	f->pixelformat = formats[0].fourcc;
	return 0;
}