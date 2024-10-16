static void cleanup_mapped_device(struct mapped_device *md)
{
	if (md->wq)
		destroy_workqueue(md->wq);
	if (md->kworker_task)
		kthread_stop(md->kworker_task);
	mempool_destroy(md->io_pool);
	if (md->bs)
		bioset_free(md->bs);

	if (md->dax_dev) {
		kill_dax(md->dax_dev);
		put_dax(md->dax_dev);
		md->dax_dev = NULL;
	}

	if (md->disk) {
		spin_lock(&_minor_lock);
		md->disk->private_data = NULL;
		spin_unlock(&_minor_lock);
		del_gendisk(md->disk);
		put_disk(md->disk);
	}

	if (md->queue)
		blk_cleanup_queue(md->queue);

	cleanup_srcu_struct(&md->io_barrier);

	if (md->bdev) {
		bdput(md->bdev);
		md->bdev = NULL;
	}

	dm_mq_cleanup_mapped_device(md);
}