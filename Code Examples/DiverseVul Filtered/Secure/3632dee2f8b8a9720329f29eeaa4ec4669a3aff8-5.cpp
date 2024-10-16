static int create_watch(struct inotify_device *dev, struct inode *inode,
			u32 mask)
{
	struct inotify_user_watch *watch;
	int ret;

	if (atomic_read(&dev->user->inotify_watches) >=
			inotify_max_user_watches)
		return -ENOSPC;

	watch = kmem_cache_alloc(watch_cachep, GFP_KERNEL);
	if (unlikely(!watch))
		return -ENOMEM;

	/* save a reference to device and bump the count to make it official */
	get_inotify_dev(dev);
	watch->dev = dev;

	atomic_inc(&dev->user->inotify_watches);

	inotify_init_watch(&watch->wdata);
	ret = inotify_add_watch(dev->ih, &watch->wdata, inode, mask);
	if (ret < 0)
		free_inotify_user_watch(&watch->wdata);

	return ret;
}