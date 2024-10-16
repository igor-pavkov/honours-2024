static inline int sector_in_part(struct hd_struct *part, sector_t sector)
{
	return part->start_sect <= sector &&
		sector < part->start_sect + part_nr_sects_read(part);
}