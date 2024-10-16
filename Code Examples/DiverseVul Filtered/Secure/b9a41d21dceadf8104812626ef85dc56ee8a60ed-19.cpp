static int __init local_init(void)
{
	int r = -ENOMEM;

	/* allocate a slab for the dm_ios */
	_io_cache = KMEM_CACHE(dm_io, 0);
	if (!_io_cache)
		return r;

	_rq_tio_cache = KMEM_CACHE(dm_rq_target_io, 0);
	if (!_rq_tio_cache)
		goto out_free_io_cache;

	_rq_cache = kmem_cache_create("dm_old_clone_request", sizeof(struct request),
				      __alignof__(struct request), 0, NULL);
	if (!_rq_cache)
		goto out_free_rq_tio_cache;

	r = dm_uevent_init();
	if (r)
		goto out_free_rq_cache;

	deferred_remove_workqueue = alloc_workqueue("kdmremove", WQ_UNBOUND, 1);
	if (!deferred_remove_workqueue) {
		r = -ENOMEM;
		goto out_uevent_exit;
	}

	_major = major;
	r = register_blkdev(_major, _name);
	if (r < 0)
		goto out_free_workqueue;

	if (!_major)
		_major = r;

	return 0;

out_free_workqueue:
	destroy_workqueue(deferred_remove_workqueue);
out_uevent_exit:
	dm_uevent_exit();
out_free_rq_cache:
	kmem_cache_destroy(_rq_cache);
out_free_rq_tio_cache:
	kmem_cache_destroy(_rq_tio_cache);
out_free_io_cache:
	kmem_cache_destroy(_io_cache);

	return r;
}