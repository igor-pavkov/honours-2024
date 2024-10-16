static void dec_pending(struct dm_io *io, blk_status_t error)
{
	unsigned long flags;
	blk_status_t io_error;
	struct bio *bio;
	struct mapped_device *md = io->md;

	/* Push-back supersedes any I/O errors */
	if (unlikely(error)) {
		spin_lock_irqsave(&io->endio_lock, flags);
		if (!(io->status == BLK_STS_DM_REQUEUE &&
				__noflush_suspending(md)))
			io->status = error;
		spin_unlock_irqrestore(&io->endio_lock, flags);
	}

	if (atomic_dec_and_test(&io->io_count)) {
		if (io->status == BLK_STS_DM_REQUEUE) {
			/*
			 * Target requested pushing back the I/O.
			 */
			spin_lock_irqsave(&md->deferred_lock, flags);
			if (__noflush_suspending(md))
				bio_list_add_head(&md->deferred, io->bio);
			else
				/* noflush suspend was interrupted. */
				io->status = BLK_STS_IOERR;
			spin_unlock_irqrestore(&md->deferred_lock, flags);
		}

		io_error = io->status;
		bio = io->bio;
		end_io_acct(io);
		free_io(md, io);

		if (io_error == BLK_STS_DM_REQUEUE)
			return;

		if ((bio->bi_opf & REQ_PREFLUSH) && bio->bi_iter.bi_size) {
			/*
			 * Preflush done for flush with data, reissue
			 * without REQ_PREFLUSH.
			 */
			bio->bi_opf &= ~REQ_PREFLUSH;
			queue_io(md, bio);
		} else {
			/* done with normal IO or empty flush */
			bio->bi_status = io_error;
			bio_endio(bio);
		}
	}
}