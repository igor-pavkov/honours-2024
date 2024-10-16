SYSCALL_DEFINE1(inotify_init1, int, flags)
{
	struct inotify_device *dev;
	struct inotify_handle *ih;
	struct user_struct *user;
	struct file *filp;
	int fd, ret;

	/* Check the IN_* constants for consistency.  */
	BUILD_BUG_ON(IN_CLOEXEC != O_CLOEXEC);
	BUILD_BUG_ON(IN_NONBLOCK != O_NONBLOCK);

	if (flags & ~(IN_CLOEXEC | IN_NONBLOCK))
		return -EINVAL;

	fd = get_unused_fd_flags(flags & O_CLOEXEC);
	if (fd < 0)
		return fd;

	filp = get_empty_filp();
	if (!filp) {
		ret = -ENFILE;
		goto out_put_fd;
	}

	user = get_current_user();
	if (unlikely(atomic_read(&user->inotify_devs) >=
			inotify_max_user_instances)) {
		ret = -EMFILE;
		goto out_free_uid;
	}

	dev = kmalloc(sizeof(struct inotify_device), GFP_KERNEL);
	if (unlikely(!dev)) {
		ret = -ENOMEM;
		goto out_free_uid;
	}

	ih = inotify_init(&inotify_user_ops);
	if (IS_ERR(ih)) {
		ret = PTR_ERR(ih);
		goto out_free_dev;
	}
	dev->ih = ih;
	dev->fa = NULL;

	filp->f_op = &inotify_fops;
	filp->f_path.mnt = mntget(inotify_mnt);
	filp->f_path.dentry = dget(inotify_mnt->mnt_root);
	filp->f_mapping = filp->f_path.dentry->d_inode->i_mapping;
	filp->f_mode = FMODE_READ;
	filp->f_flags = O_RDONLY | (flags & O_NONBLOCK);
	filp->private_data = dev;

	INIT_LIST_HEAD(&dev->events);
	init_waitqueue_head(&dev->wq);
	mutex_init(&dev->ev_mutex);
	mutex_init(&dev->up_mutex);
	dev->event_count = 0;
	dev->queue_size = 0;
	dev->max_events = inotify_max_queued_events;
	dev->user = user;
	atomic_set(&dev->count, 0);

	get_inotify_dev(dev);
	atomic_inc(&user->inotify_devs);
	fd_install(fd, filp);

	return fd;
out_free_dev:
	kfree(dev);
out_free_uid:
	free_uid(user);
	put_filp(filp);
out_put_fd:
	put_unused_fd(fd);
	return ret;
}