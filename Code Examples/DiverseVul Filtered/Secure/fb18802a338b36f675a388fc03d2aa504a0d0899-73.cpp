static int v4l_s_fmt(const struct v4l2_ioctl_ops *ops,
				struct file *file, void *fh, void *arg)
{
	struct v4l2_format *p = arg;
	struct video_device *vfd = video_devdata(file);
	int ret = check_fmt(file, p->type);
	unsigned int i;

	if (ret)
		return ret;

	ret = v4l_enable_media_source(vfd);
	if (ret)
		return ret;
	v4l_sanitize_format(p);

	switch (p->type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		if (unlikely(!ops->vidioc_s_fmt_vid_cap))
			break;
		CLEAR_AFTER_FIELD(p, fmt.pix);
		ret = ops->vidioc_s_fmt_vid_cap(file, fh, arg);
		/* just in case the driver zeroed it again */
		p->fmt.pix.priv = V4L2_PIX_FMT_PRIV_MAGIC;
		if (vfd->vfl_type == VFL_TYPE_TOUCH)
			v4l_pix_format_touch(&p->fmt.pix);
		return ret;
	case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
		if (unlikely(!ops->vidioc_s_fmt_vid_cap_mplane))
			break;
		CLEAR_AFTER_FIELD(p, fmt.pix_mp.xfer_func);
		for (i = 0; i < p->fmt.pix_mp.num_planes; i++)
			CLEAR_AFTER_FIELD(&p->fmt.pix_mp.plane_fmt[i],
					  bytesperline);
		return ops->vidioc_s_fmt_vid_cap_mplane(file, fh, arg);
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
		if (unlikely(!ops->vidioc_s_fmt_vid_overlay))
			break;
		CLEAR_AFTER_FIELD(p, fmt.win);
		return ops->vidioc_s_fmt_vid_overlay(file, fh, arg);
	case V4L2_BUF_TYPE_VBI_CAPTURE:
		if (unlikely(!ops->vidioc_s_fmt_vbi_cap))
			break;
		CLEAR_AFTER_FIELD(p, fmt.vbi.flags);
		return ops->vidioc_s_fmt_vbi_cap(file, fh, arg);
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
		if (unlikely(!ops->vidioc_s_fmt_sliced_vbi_cap))
			break;
		CLEAR_AFTER_FIELD(p, fmt.sliced.io_size);
		return ops->vidioc_s_fmt_sliced_vbi_cap(file, fh, arg);
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
		if (unlikely(!ops->vidioc_s_fmt_vid_out))
			break;
		CLEAR_AFTER_FIELD(p, fmt.pix);
		ret = ops->vidioc_s_fmt_vid_out(file, fh, arg);
		/* just in case the driver zeroed it again */
		p->fmt.pix.priv = V4L2_PIX_FMT_PRIV_MAGIC;
		return ret;
	case V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE:
		if (unlikely(!ops->vidioc_s_fmt_vid_out_mplane))
			break;
		CLEAR_AFTER_FIELD(p, fmt.pix_mp.xfer_func);
		for (i = 0; i < p->fmt.pix_mp.num_planes; i++)
			CLEAR_AFTER_FIELD(&p->fmt.pix_mp.plane_fmt[i],
					  bytesperline);
		return ops->vidioc_s_fmt_vid_out_mplane(file, fh, arg);
	case V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY:
		if (unlikely(!ops->vidioc_s_fmt_vid_out_overlay))
			break;
		CLEAR_AFTER_FIELD(p, fmt.win);
		return ops->vidioc_s_fmt_vid_out_overlay(file, fh, arg);
	case V4L2_BUF_TYPE_VBI_OUTPUT:
		if (unlikely(!ops->vidioc_s_fmt_vbi_out))
			break;
		CLEAR_AFTER_FIELD(p, fmt.vbi.flags);
		return ops->vidioc_s_fmt_vbi_out(file, fh, arg);
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		if (unlikely(!ops->vidioc_s_fmt_sliced_vbi_out))
			break;
		CLEAR_AFTER_FIELD(p, fmt.sliced.io_size);
		return ops->vidioc_s_fmt_sliced_vbi_out(file, fh, arg);
	case V4L2_BUF_TYPE_SDR_CAPTURE:
		if (unlikely(!ops->vidioc_s_fmt_sdr_cap))
			break;
		CLEAR_AFTER_FIELD(p, fmt.sdr.buffersize);
		return ops->vidioc_s_fmt_sdr_cap(file, fh, arg);
	case V4L2_BUF_TYPE_SDR_OUTPUT:
		if (unlikely(!ops->vidioc_s_fmt_sdr_out))
			break;
		CLEAR_AFTER_FIELD(p, fmt.sdr.buffersize);
		return ops->vidioc_s_fmt_sdr_out(file, fh, arg);
	case V4L2_BUF_TYPE_META_CAPTURE:
		if (unlikely(!ops->vidioc_s_fmt_meta_cap))
			break;
		CLEAR_AFTER_FIELD(p, fmt.meta);
		return ops->vidioc_s_fmt_meta_cap(file, fh, arg);
	case V4L2_BUF_TYPE_META_OUTPUT:
		if (unlikely(!ops->vidioc_s_fmt_meta_out))
			break;
		CLEAR_AFTER_FIELD(p, fmt.meta);
		return ops->vidioc_s_fmt_meta_out(file, fh, arg);
	}
	return -EINVAL;
}