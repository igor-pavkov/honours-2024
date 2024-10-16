void blkdev_show(struct seq_file *seqf, off_t offset)
{
	struct blk_major_name *dp;

	if (offset < BLKDEV_MAJOR_HASH_SIZE) {
		mutex_lock(&block_class_lock);
		for (dp = major_names[offset]; dp; dp = dp->next)
			seq_printf(seqf, "%3d %s\n", dp->major, dp->name);
		mutex_unlock(&block_class_lock);
	}
}