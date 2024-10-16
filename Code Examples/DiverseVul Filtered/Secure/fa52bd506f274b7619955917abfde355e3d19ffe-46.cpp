static int vidioc_enum_fmt_vid_cap(struct file *file, void  *priv,
					struct v4l2_fmtdesc *vfd)
{
	if (vfd->index >= USBVISION_SUPPORTED_PALETTES - 1)
		return -EINVAL;
	strcpy(vfd->description, usbvision_v4l2_format[vfd->index].desc);
	vfd->pixelformat = usbvision_v4l2_format[vfd->index].format;
	return 0;
}