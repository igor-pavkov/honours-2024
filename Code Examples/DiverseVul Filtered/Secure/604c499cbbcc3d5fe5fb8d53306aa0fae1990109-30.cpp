int xen_blkif_schedule(void *arg)
{
	struct xen_blkif *blkif = arg;
	struct xen_vbd *vbd = &blkif->vbd;
	unsigned long timeout;

	xen_blkif_get(blkif);

	while (!kthread_should_stop()) {
		if (try_to_freeze())
			continue;
		if (unlikely(vbd->size != vbd_sz(vbd)))
			xen_vbd_resize(blkif);

		timeout = msecs_to_jiffies(LRU_INTERVAL);

		timeout = wait_event_interruptible_timeout(
			blkif->wq,
			blkif->waiting_reqs || kthread_should_stop(),
			timeout);
		if (timeout == 0)
			goto purge_gnt_list;
		timeout = wait_event_interruptible_timeout(
			blkif->pending_free_wq,
			!list_empty(&blkif->pending_free) ||
			kthread_should_stop(),
			timeout);
		if (timeout == 0)
			goto purge_gnt_list;

		blkif->waiting_reqs = 0;
		smp_mb(); /* clear flag *before* checking for work */

		if (do_block_io_op(blkif))
			blkif->waiting_reqs = 1;

purge_gnt_list:
		if (blkif->vbd.feature_gnt_persistent &&
		    time_after(jiffies, blkif->next_lru)) {
			purge_persistent_gnt(blkif);
			blkif->next_lru = jiffies + msecs_to_jiffies(LRU_INTERVAL);
		}

		/* Shrink if we have more than xen_blkif_max_buffer_pages */
		shrink_free_pagepool(blkif, xen_blkif_max_buffer_pages);

		if (log_stats && time_after(jiffies, blkif->st_print))
			print_stats(blkif);
	}

	/* Since we are shutting down remove all pages from the buffer */
	shrink_free_pagepool(blkif, 0 /* All */);

	/* Free all persistent grant pages */
	if (!RB_EMPTY_ROOT(&blkif->persistent_gnts))
		free_persistent_gnts(blkif, &blkif->persistent_gnts,
			blkif->persistent_gnt_c);

	BUG_ON(!RB_EMPTY_ROOT(&blkif->persistent_gnts));
	blkif->persistent_gnt_c = 0;

	if (log_stats)
		print_stats(blkif);

	blkif->xenblkd = NULL;
	xen_blkif_put(blkif);

	return 0;
}