static long pipe_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct pipe_inode_info *pipe = filp->private_data;
	unsigned int count, head, tail, mask;

	switch (cmd) {
	case FIONREAD:
		__pipe_lock(pipe);
		count = 0;
		head = pipe->head;
		tail = pipe->tail;
		mask = pipe->ring_size - 1;

		while (tail != head) {
			count += pipe->bufs[tail & mask].len;
			tail++;
		}
		__pipe_unlock(pipe);

		return put_user(count, (int __user *)arg);

#ifdef CONFIG_WATCH_QUEUE
	case IOC_WATCH_QUEUE_SET_SIZE: {
		int ret;
		__pipe_lock(pipe);
		ret = watch_queue_set_size(pipe, arg);
		__pipe_unlock(pipe);
		return ret;
	}

	case IOC_WATCH_QUEUE_SET_FILTER:
		return watch_queue_set_filter(
			pipe, (struct watch_notification_filter __user *)arg);
#endif

	default:
		return -ENOIOCTLCMD;
	}
}