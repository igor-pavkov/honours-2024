int disk_expand_part_tbl(struct gendisk *disk, int partno)
{
	struct disk_part_tbl *old_ptbl = disk->part_tbl;
	struct disk_part_tbl *new_ptbl;
	int len = old_ptbl ? old_ptbl->len : 0;
	int i, target;
	size_t size;

	/*
	 * check for int overflow, since we can get here from blkpg_ioctl()
	 * with a user passed 'partno'.
	 */
	target = partno + 1;
	if (target < 0)
		return -EINVAL;

	/* disk_max_parts() is zero during initialization, ignore if so */
	if (disk_max_parts(disk) && target > disk_max_parts(disk))
		return -EINVAL;

	if (target <= len)
		return 0;

	size = sizeof(*new_ptbl) + target * sizeof(new_ptbl->part[0]);
	new_ptbl = kzalloc_node(size, GFP_KERNEL, disk->node_id);
	if (!new_ptbl)
		return -ENOMEM;

	new_ptbl->len = target;

	for (i = 0; i < len; i++)
		rcu_assign_pointer(new_ptbl->part[i], old_ptbl->part[i]);

	disk_replace_part_tbl(disk, new_ptbl);
	return 0;
}