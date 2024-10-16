static int vidioc_querycap(struct file *file, void *priv, struct v4l2_capability *cap)
{
	struct v4l2_loopback_device *dev = v4l2loopback_getdevice(file);
	int devnr = ((struct v4l2loopback_private *)video_get_drvdata(dev->vdev))->devicenr;
        __u32 capabilities = V4L2_CAP_STREAMING | V4L2_CAP_READWRITE;

	strlcpy(cap->driver, "v4l2 loopback", sizeof(cap->driver));
	vidioc_fill_name(cap->card, sizeof(cap->card), devnr);
	snprintf(cap->bus_info, sizeof(cap->bus_info), "platform:v4l2loopback-%03d", devnr);

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 1, 0)
	/* since 3.1.0, the v4l2-core system is supposed to set the version */
	cap->version = V4L2LOOPBACK_VERSION_CODE;
#endif

#ifdef V4L2_CAP_VIDEO_M2M
	capabilities |= V4L2_CAP_VIDEO_M2M;
#endif /* V4L2_CAP_VIDEO_M2M */

	if (dev->announce_all_caps) {
		capabilities |= V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_VIDEO_OUTPUT;
	} else {

		if (dev->ready_for_capture) {
			capabilities |= V4L2_CAP_VIDEO_CAPTURE;
		}
		if (dev->ready_for_output) {
			capabilities |= V4L2_CAP_VIDEO_OUTPUT;
		}
	}

	dev->vdev->device_caps = cap->device_caps = cap->capabilities = capabilities;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)
	cap->capabilities |= V4L2_CAP_DEVICE_CAPS;
#endif

	memset(cap->reserved, 0, sizeof(cap->reserved));
	return 0;
}