int dm_setup_md_queue(struct mapped_device *md, struct dm_table *t)
{
	int r;
	enum dm_queue_mode type = dm_get_md_type(md);

	switch (type) {
	case DM_TYPE_REQUEST_BASED:
		r = dm_old_init_request_queue(md, t);
		if (r) {
			DMERR("Cannot initialize queue for request-based mapped device");
			return r;
		}
		break;
	case DM_TYPE_MQ_REQUEST_BASED:
		r = dm_mq_init_request_queue(md, t);
		if (r) {
			DMERR("Cannot initialize queue for request-based dm-mq mapped device");
			return r;
		}
		break;
	case DM_TYPE_BIO_BASED:
	case DM_TYPE_DAX_BIO_BASED:
		dm_init_normal_md_queue(md);
		blk_queue_make_request(md->queue, dm_make_request);
		/*
		 * DM handles splitting bios as needed.  Free the bio_split bioset
		 * since it won't be used (saves 1 process per bio-based DM device).
		 */
		bioset_free(md->queue->bio_split);
		md->queue->bio_split = NULL;

		if (type == DM_TYPE_DAX_BIO_BASED)
			queue_flag_set_unlocked(QUEUE_FLAG_DAX, md->queue);
		break;
	case DM_TYPE_NONE:
		WARN_ON_ONCE(true);
		break;
	}

	return 0;
}