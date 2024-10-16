static struct dm_target_io *alloc_tio(struct clone_info *ci,
				      struct dm_target *ti,
				      unsigned target_bio_nr)
{
	struct dm_target_io *tio;
	struct bio *clone;

	clone = bio_alloc_bioset(GFP_NOIO, 0, ci->md->bs);
	tio = container_of(clone, struct dm_target_io, clone);

	tio->io = ci->io;
	tio->ti = ti;
	tio->target_bio_nr = target_bio_nr;

	return tio;
}