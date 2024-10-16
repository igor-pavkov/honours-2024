static int vidioc_streamon(struct file *file, void *private_data, enum v4l2_buf_type type)
{
	struct v4l2_loopback_device *dev;
	struct v4l2_loopback_opener *opener;
	MARK();

	dev = v4l2loopback_getdevice(file);
	opener = file->private_data;

	switch (type) {
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
		opener->type = WRITER;
		dev->ready_for_output = 0;
		if (!dev->ready_for_capture) {
			int ret = allocate_buffers(dev);
			if (ret < 0)
				return ret;
			dev->ready_for_capture = 1;
		}
		return 0;
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		opener->type = READER;
		if (!dev->ready_for_capture)
			return -EIO;
		return 0;
	default:
		return -EINVAL;
	}
        return -EINVAL;
}