void dm_init_md_queue(struct mapped_device *md)
{
	/*
	 * Request-based dm devices cannot be stacked on top of bio-based dm
	 * devices.  The type of this dm device may not have been decided yet.
	 * The type is decided at the first table loading time.
	 * To prevent problematic device stacking, clear the queue flag
	 * for request stacking support until then.
	 *
	 * This queue is new, so no concurrency on the queue_flags.
	 */
	queue_flag_clear_unlocked(QUEUE_FLAG_STACKABLE, md->queue);

	/*
	 * Initialize data that will only be used by a non-blk-mq DM queue
	 * - must do so here (in alloc_dev callchain) before queue is used
	 */
	md->queue->queuedata = md;
	md->queue->backing_dev_info->congested_data = md;
}