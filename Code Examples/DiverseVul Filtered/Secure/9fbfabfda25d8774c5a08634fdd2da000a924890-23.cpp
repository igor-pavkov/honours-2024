void blk_drop_partitions(struct gendisk *disk)
{
	struct block_device *part;
	unsigned long idx;

	lockdep_assert_held(&disk->open_mutex);

	xa_for_each_start(&disk->part_tbl, idx, part, 1)
		delete_partition(part);
}