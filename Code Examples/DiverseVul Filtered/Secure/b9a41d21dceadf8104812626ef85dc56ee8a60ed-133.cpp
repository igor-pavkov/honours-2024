static void __set_size(struct mapped_device *md, sector_t size)
{
	lockdep_assert_held(&md->suspend_lock);

	set_capacity(md->disk, size);

	i_size_write(md->bdev->bd_inode, (loff_t)size << SECTOR_SHIFT);
}