static inline void put_inotify_dev(struct inotify_device *dev)
{
	if (atomic_dec_and_test(&dev->count)) {
		atomic_dec(&dev->user->inotify_devs);
		free_uid(dev->user);
		kfree(dev);
	}
}