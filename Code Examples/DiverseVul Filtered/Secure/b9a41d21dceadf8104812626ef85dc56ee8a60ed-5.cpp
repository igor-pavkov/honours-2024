static void __map_bio(struct dm_target_io *tio)
{
	int r;
	sector_t sector;
	struct dm_offload o;
	struct bio *clone = &tio->clone;
	struct dm_target *ti = tio->ti;

	clone->bi_end_io = clone_endio;

	/*
	 * Map the clone.  If r == 0 we don't need to do
	 * anything, the target has assumed ownership of
	 * this io.
	 */
	atomic_inc(&tio->io->io_count);
	sector = clone->bi_iter.bi_sector;

	dm_offload_start(&o);
	r = ti->type->map(ti, clone);
	dm_offload_end(&o);

	switch (r) {
	case DM_MAPIO_SUBMITTED:
		break;
	case DM_MAPIO_REMAPPED:
		/* the bio has been remapped so dispatch it */
		trace_block_bio_remap(clone->bi_disk->queue, clone,
				      bio_dev(tio->io->bio), sector);
		generic_make_request(clone);
		break;
	case DM_MAPIO_KILL:
		dec_pending(tio->io, BLK_STS_IOERR);
		free_tio(tio);
		break;
	case DM_MAPIO_REQUEUE:
		dec_pending(tio->io, BLK_STS_DM_REQUEUE);
		free_tio(tio);
		break;
	default:
		DMWARN("unimplemented target map return value: %d", r);
		BUG();
	}
}