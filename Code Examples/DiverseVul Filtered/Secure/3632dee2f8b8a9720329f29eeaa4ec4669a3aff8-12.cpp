static void free_inotify_user_watch(struct inotify_watch *w)
{
	struct inotify_user_watch *watch;
	struct inotify_device *dev;

	watch = container_of(w, struct inotify_user_watch, wdata);
	dev = watch->dev;

	atomic_dec(&dev->user->inotify_watches);
	put_inotify_dev(dev);
	kmem_cache_free(watch_cachep, watch);
}