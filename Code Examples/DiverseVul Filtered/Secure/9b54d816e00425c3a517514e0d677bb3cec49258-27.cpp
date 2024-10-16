	__acquires(rcu) __acquires(disk->queue->queue_lock)
{
	struct gendisk *disk;
	struct blkcg_gq *blkg;
	struct module *owner;
	unsigned int major, minor;
	int key_len, part, ret;
	char *body;

	if (sscanf(input, "%u:%u%n", &major, &minor, &key_len) != 2)
		return -EINVAL;

	body = input + key_len;
	if (!isspace(*body))
		return -EINVAL;
	body = skip_spaces(body);

	disk = get_gendisk(MKDEV(major, minor), &part);
	if (!disk)
		return -ENODEV;
	if (part) {
		owner = disk->fops->owner;
		put_disk(disk);
		module_put(owner);
		return -ENODEV;
	}

	rcu_read_lock();
	spin_lock_irq(disk->queue->queue_lock);

	if (blkcg_policy_enabled(disk->queue, pol))
		blkg = blkg_lookup_create(blkcg, disk->queue);
	else
		blkg = ERR_PTR(-EOPNOTSUPP);

	if (IS_ERR(blkg)) {
		ret = PTR_ERR(blkg);
		rcu_read_unlock();
		spin_unlock_irq(disk->queue->queue_lock);
		owner = disk->fops->owner;
		put_disk(disk);
		module_put(owner);
		/*
		 * If queue was bypassing, we should retry.  Do so after a
		 * short msleep().  It isn't strictly necessary but queue
		 * can be bypassing for some time and it's always nice to
		 * avoid busy looping.
		 */
		if (ret == -EBUSY) {
			msleep(10);
			ret = restart_syscall();
		}
		return ret;
	}

	ctx->disk = disk;
	ctx->blkg = blkg;
	ctx->body = body;
	return 0;
}