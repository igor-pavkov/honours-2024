static int snd_seq_ioctl_get_queue_info(struct snd_seq_client *client,
					void __user *arg)
{
	struct snd_seq_queue_info info;
	struct snd_seq_queue *q;

	if (copy_from_user(&info, arg, sizeof(info)))
		return -EFAULT;

	q = queueptr(info.queue);
	if (q == NULL)
		return -EINVAL;

	memset(&info, 0, sizeof(info));
	info.queue = q->queue;
	info.owner = q->owner;
	info.locked = q->locked;
	strlcpy(info.name, q->name, sizeof(info.name));
	queuefree(q);

	if (copy_to_user(arg, &info, sizeof(info)))
		return -EFAULT;

	return 0;
}