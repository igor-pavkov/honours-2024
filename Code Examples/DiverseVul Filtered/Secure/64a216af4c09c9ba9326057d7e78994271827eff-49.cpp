static int allocate_buffers(struct v4l2_loopback_device *dev)
{
	MARK();
	/* vfree on close file operation in case no open handles left */
	if (0 == dev->buffer_size)
		return -EINVAL;

	if (dev->image) {
		dprintk("allocating buffers again: %ld %ld\n",
			dev->buffer_size * dev->buffers_number, dev->imagesize);
		/* FIXME: prevent double allocation more intelligently! */
		if (dev->buffer_size * dev->buffers_number == dev->imagesize)
			return 0;

		/* if there is only one writer, no problem should occur */
		if (dev->open_count.counter == 1)
			free_buffers(dev);
		else
			return -EINVAL;
	}

	dev->imagesize = dev->buffer_size * dev->buffers_number;

	dprintk("allocating %ld = %ldx%d\n", dev->imagesize, dev->buffer_size, dev->buffers_number);

	dev->image = vmalloc(dev->imagesize);
	if (dev->timeout_jiffies > 0)
		allocate_timeout_image(dev);

	if (dev->image == NULL)
		return -ENOMEM;
	dprintk("vmallocated %ld bytes\n", dev->imagesize);
	MARK();
	init_buffers(dev);
	return 0;
}