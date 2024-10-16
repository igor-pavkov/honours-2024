static void delete_partition(struct block_device *part)
{
	lockdep_assert_held(&part->bd_disk->open_mutex);

	fsync_bdev(part);
	__invalidate_device(part, true);

	xa_erase(&part->bd_disk->part_tbl, part->bd_partno);
	kobject_put(part->bd_holder_dir);
	device_del(&part->bd_device);

	/*
	 * Remove the block device from the inode hash, so that it cannot be
	 * looked up any more even when openers still hold references.
	 */
	remove_inode_hash(part->bd_inode);

	put_device(&part->bd_device);
}