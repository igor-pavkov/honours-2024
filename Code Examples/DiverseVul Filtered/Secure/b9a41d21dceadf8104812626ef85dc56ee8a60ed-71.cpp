static int clone_bio(struct dm_target_io *tio, struct bio *bio,
		     sector_t sector, unsigned len)
{
	struct bio *clone = &tio->clone;

	__bio_clone_fast(clone, bio);

	if (unlikely(bio_integrity(bio) != NULL)) {
		int r;

		if (unlikely(!dm_target_has_integrity(tio->ti->type) &&
			     !dm_target_passes_integrity(tio->ti->type))) {
			DMWARN("%s: the target %s doesn't support integrity data.",
				dm_device_name(tio->io->md),
				tio->ti->type->name);
			return -EIO;
		}

		r = bio_integrity_clone(clone, bio, GFP_NOIO);
		if (r < 0)
			return r;
	}

	if (bio_op(bio) != REQ_OP_ZONE_REPORT)
		bio_advance(clone, to_bytes(sector - clone->bi_iter.bi_sector));
	clone->bi_iter.bi_size = to_bytes(len);

	if (unlikely(bio_integrity(bio) != NULL))
		bio_integrity_trim(clone);

	return 0;
}