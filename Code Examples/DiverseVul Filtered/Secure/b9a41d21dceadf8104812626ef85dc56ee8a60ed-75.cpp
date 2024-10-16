static int __clone_and_map_data_bio(struct clone_info *ci, struct dm_target *ti,
				     sector_t sector, unsigned *len)
{
	struct bio *bio = ci->bio;
	struct dm_target_io *tio;
	unsigned target_bio_nr;
	unsigned num_target_bios = 1;
	int r = 0;

	/*
	 * Does the target want to receive duplicate copies of the bio?
	 */
	if (bio_data_dir(bio) == WRITE && ti->num_write_bios)
		num_target_bios = ti->num_write_bios(ti, bio);

	for (target_bio_nr = 0; target_bio_nr < num_target_bios; target_bio_nr++) {
		tio = alloc_tio(ci, ti, target_bio_nr);
		tio->len_ptr = len;
		r = clone_bio(tio, bio, sector, *len);
		if (r < 0) {
			free_tio(tio);
			break;
		}
		__map_bio(tio);
	}

	return r;
}