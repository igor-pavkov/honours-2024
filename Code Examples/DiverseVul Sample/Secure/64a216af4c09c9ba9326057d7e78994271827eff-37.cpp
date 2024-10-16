static struct v4l2_loopback_device *v4l2loopback_cd2dev(struct device *cd)
{
	struct video_device *loopdev = to_video_device(cd);
	struct v4l2loopback_private *ptr =
		(struct v4l2loopback_private *)video_get_drvdata(loopdev);
	int nr = ptr->devicenr;

	if (nr < 0 || nr >= devices) {
		printk(KERN_ERR "v4l2-loopback: illegal device %d\n", nr);
		return NULL;
	}
	return devs[nr];
}