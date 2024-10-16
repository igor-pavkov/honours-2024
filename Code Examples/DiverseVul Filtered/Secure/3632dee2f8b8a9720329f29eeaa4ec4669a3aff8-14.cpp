static inline void get_inotify_dev(struct inotify_device *dev)
{
	atomic_inc(&dev->count);
}