static int v4l2_loopback_open(struct file *file)
{
	struct v4l2_loopback_device *dev;
	struct v4l2_loopback_opener *opener;
	MARK();

	dev = v4l2loopback_getdevice(file);

	if (dev->open_count.counter >= dev->max_openers)
		return -EBUSY;
	/* kfree on close */
	opener = kzalloc(sizeof(*opener), GFP_KERNEL);
	if (opener == NULL)
		return -ENOMEM;
	file->private_data = opener;
	atomic_inc(&dev->open_count);

	opener->timeout_image_io = dev->timeout_image_io;
	dev->timeout_image_io = 0;

	if (opener->timeout_image_io) {
		int r = allocate_timeout_image(dev);

		if (r < 0) {
			dprintk("timeout image allocation failed\n");
			return r;
		}
	}
	dprintk("opened dev:%p with image:%p\n", dev, dev ? dev->image : NULL);
	MARK();
	return 0;
}