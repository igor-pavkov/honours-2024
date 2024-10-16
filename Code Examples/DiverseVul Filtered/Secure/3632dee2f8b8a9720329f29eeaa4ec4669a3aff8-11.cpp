static int __init inotify_user_setup(void)
{
	int ret;

	ret = register_filesystem(&inotify_fs_type);
	if (unlikely(ret))
		panic("inotify: register_filesystem returned %d!\n", ret);

	inotify_mnt = kern_mount(&inotify_fs_type);
	if (IS_ERR(inotify_mnt))
		panic("inotify: kern_mount ret %ld!\n", PTR_ERR(inotify_mnt));

	inotify_max_queued_events = 16384;
	inotify_max_user_instances = 128;
	inotify_max_user_watches = 8192;

	watch_cachep = kmem_cache_create("inotify_watch_cache",
					 sizeof(struct inotify_user_watch),
					 0, SLAB_PANIC, NULL);
	event_cachep = kmem_cache_create("inotify_event_cache",
					 sizeof(struct inotify_kernel_event),
					 0, SLAB_PANIC, NULL);

	return 0;
}