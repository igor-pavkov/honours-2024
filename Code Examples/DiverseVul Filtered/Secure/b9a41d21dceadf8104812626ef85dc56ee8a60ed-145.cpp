static void __clone_and_map_simple_bio(struct clone_info *ci,
				       struct dm_target *ti,
				       unsigned target_bio_nr, unsigned *len)
{
	struct dm_target_io *tio = alloc_tio(ci, ti, target_bio_nr);
	struct bio *clone = &tio->clone;

	tio->len_ptr = len;

	__bio_clone_fast(clone, ci->bio);
	if (len)
		bio_setup_sector(clone, ci->sector, *len);

	__map_bio(tio);
}