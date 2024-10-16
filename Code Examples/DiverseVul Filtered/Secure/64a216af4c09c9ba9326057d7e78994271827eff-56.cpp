static ssize_t v4l2_loopback_write(struct file *file,
		const char __user *buf, size_t count, loff_t *ppos)
{
	struct v4l2_loopback_device *dev;
	int write_index;
	struct v4l2_buffer *b;
	MARK();

	dev = v4l2loopback_getdevice(file);

	/* there's at least one writer, so don'stop announcing output capabilities */
	dev->ready_for_output = 0;

	if (!dev->ready_for_capture) {
		int ret = allocate_buffers(dev);
		if (ret < 0)
			return ret;
		dev->ready_for_capture = 1;
	}
	dprintkrw("v4l2_loopback_write() trying to write %zu bytes\n", count);
	if (count > dev->buffer_size)
		count = dev->buffer_size;

	write_index = dev->write_position % dev->used_buffers;
	b = &dev->buffers[write_index].buffer;

	if (copy_from_user((void *)(dev->image + b->m.offset), (void *)buf, count)) {
		printk(KERN_ERR
			"v4l2-loopback: failed copy_from_user() in write buf, could not write %zu\n",
			count);
		return -EFAULT;
	}
	v4l2l_get_timestamp(b);
	b->bytesused = count;
	b->sequence = dev->write_position;
	buffer_written(dev, &dev->buffers[write_index]);
	wake_up_all(&dev->read_event);
	dprintkrw("leave v4l2_loopback_write()\n");
	return count;
}