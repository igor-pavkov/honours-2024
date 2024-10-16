static void __send_duplicate_bios(struct clone_info *ci, struct dm_target *ti,
				  unsigned num_bios, unsigned *len)
{
	unsigned target_bio_nr;

	for (target_bio_nr = 0; target_bio_nr < num_bios; target_bio_nr++)
		__clone_and_map_simple_bio(ci, ti, target_bio_nr, len);
}