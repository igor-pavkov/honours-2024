static blk_qc_t dm_make_request(struct request_queue *q, struct bio *bio)
{
	int rw = bio_data_dir(bio);
	struct mapped_device *md = q->queuedata;
	int srcu_idx;
	struct dm_table *map;

	map = dm_get_live_table(md, &srcu_idx);

	generic_start_io_acct(q, rw, bio_sectors(bio), &dm_disk(md)->part0);

	/* if we're suspended, we have to queue this io for later */
	if (unlikely(test_bit(DMF_BLOCK_IO_FOR_SUSPEND, &md->flags))) {
		dm_put_live_table(md, srcu_idx);

		if (!(bio->bi_opf & REQ_RAHEAD))
			queue_io(md, bio);
		else
			bio_io_error(bio);
		return BLK_QC_T_NONE;
	}

	__split_and_process_bio(md, map, bio);
	dm_put_live_table(md, srcu_idx);
	return BLK_QC_T_NONE;
}