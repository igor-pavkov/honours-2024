static int vidioc_try_fmt_vid_cap(struct file *file, void *priv,
			       struct v4l2_format *vf)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	int format_idx;

	/* Find requested format in available ones */
	for (format_idx = 0; format_idx < USBVISION_SUPPORTED_PALETTES; format_idx++) {
		if (vf->fmt.pix.pixelformat ==
		   usbvision_v4l2_format[format_idx].format) {
			usbvision->palette = usbvision_v4l2_format[format_idx];
			break;
		}
	}
	/* robustness */
	if (format_idx == USBVISION_SUPPORTED_PALETTES)
		return -EINVAL;
	RESTRICT_TO_RANGE(vf->fmt.pix.width, MIN_FRAME_WIDTH, MAX_FRAME_WIDTH);
	RESTRICT_TO_RANGE(vf->fmt.pix.height, MIN_FRAME_HEIGHT, MAX_FRAME_HEIGHT);

	vf->fmt.pix.bytesperline = vf->fmt.pix.width*
		usbvision->palette.bytes_per_pixel;
	vf->fmt.pix.sizeimage = vf->fmt.pix.bytesperline*vf->fmt.pix.height;
	vf->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;
	vf->fmt.pix.field = V4L2_FIELD_NONE; /* Always progressive image */

	return 0;
}