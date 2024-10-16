static void __split_and_process_bio(struct mapped_device *md,
				    struct dm_table *map, struct bio *bio)
{
	struct clone_info ci;
	int error = 0;

	if (unlikely(!map)) {
		bio_io_error(bio);
		return;
	}

	ci.map = map;
	ci.md = md;
	ci.io = alloc_io(md);
	ci.io->status = 0;
	atomic_set(&ci.io->io_count, 1);
	ci.io->bio = bio;
	ci.io->md = md;
	spin_lock_init(&ci.io->endio_lock);
	ci.sector = bio->bi_iter.bi_sector;

	start_io_acct(ci.io);

	if (bio->bi_opf & REQ_PREFLUSH) {
		ci.bio = &ci.md->flush_bio;
		ci.sector_count = 0;
		error = __send_empty_flush(&ci);
		/* dec_pending submits any data associated with flush */
	} else if (bio_op(bio) == REQ_OP_ZONE_RESET) {
		ci.bio = bio;
		ci.sector_count = 0;
		error = __split_and_process_non_flush(&ci);
	} else {
		ci.bio = bio;
		ci.sector_count = bio_sectors(bio);
		while (ci.sector_count && !error)
			error = __split_and_process_non_flush(&ci);
	}

	/* drop the extra reference count */
	dec_pending(ci.io, errno_to_blk_status(error));
}