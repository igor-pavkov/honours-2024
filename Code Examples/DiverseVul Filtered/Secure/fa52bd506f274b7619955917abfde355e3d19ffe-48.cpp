static int vidioc_g_fmt_vid_cap(struct file *file, void *priv,
					struct v4l2_format *vf)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	vf->fmt.pix.width = usbvision->curwidth;
	vf->fmt.pix.height = usbvision->curheight;
	vf->fmt.pix.pixelformat = usbvision->palette.format;
	vf->fmt.pix.bytesperline =
		usbvision->curwidth * usbvision->palette.bytes_per_pixel;
	vf->fmt.pix.sizeimage = vf->fmt.pix.bytesperline * usbvision->curheight;
	vf->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;
	vf->fmt.pix.field = V4L2_FIELD_NONE; /* Always progressive image */

	return 0;
}