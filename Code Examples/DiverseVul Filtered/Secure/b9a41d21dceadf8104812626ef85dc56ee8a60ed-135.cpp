static void clone_endio(struct bio *bio)
{
	blk_status_t error = bio->bi_status;
	struct dm_target_io *tio = container_of(bio, struct dm_target_io, clone);
	struct dm_io *io = tio->io;
	struct mapped_device *md = tio->io->md;
	dm_endio_fn endio = tio->ti->type->end_io;

	if (unlikely(error == BLK_STS_TARGET)) {
		if (bio_op(bio) == REQ_OP_WRITE_SAME &&
		    !bio->bi_disk->queue->limits.max_write_same_sectors)
			disable_write_same(md);
		if (bio_op(bio) == REQ_OP_WRITE_ZEROES &&
		    !bio->bi_disk->queue->limits.max_write_zeroes_sectors)
			disable_write_zeroes(md);
	}

	if (endio) {
		int r = endio(tio->ti, bio, &error);
		switch (r) {
		case DM_ENDIO_REQUEUE:
			error = BLK_STS_DM_REQUEUE;
			/*FALLTHRU*/
		case DM_ENDIO_DONE:
			break;
		case DM_ENDIO_INCOMPLETE:
			/* The target will handle the io */
			return;
		default:
			DMWARN("unimplemented target endio return value: %d", r);
			BUG();
		}
	}

	free_tio(tio);
	dec_pending(io, error);
}