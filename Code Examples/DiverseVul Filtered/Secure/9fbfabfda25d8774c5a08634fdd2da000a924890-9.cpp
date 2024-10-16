static bool partition_overlaps(struct gendisk *disk, sector_t start,
		sector_t length, int skip_partno)
{
	struct block_device *part;
	bool overlap = false;
	unsigned long idx;

	rcu_read_lock();
	xa_for_each_start(&disk->part_tbl, idx, part, 1) {
		if (part->bd_partno != skip_partno &&
		    start < part->bd_start_sect + bdev_nr_sectors(part) &&
		    start + length > part->bd_start_sect) {
			overlap = true;
			break;
		}
	}
	rcu_read_unlock();

	return overlap;
}