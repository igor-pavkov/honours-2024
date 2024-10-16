struct hd_struct *disk_get_part(struct gendisk *disk, int partno)
{
	struct hd_struct *part = NULL;
	struct disk_part_tbl *ptbl;

	if (unlikely(partno < 0))
		return NULL;

	rcu_read_lock();

	ptbl = rcu_dereference(disk->part_tbl);
	if (likely(partno < ptbl->len)) {
		part = rcu_dereference(ptbl->part[partno]);
		if (part)
			get_device(part_to_dev(part));
	}

	rcu_read_unlock();

	return part;
}