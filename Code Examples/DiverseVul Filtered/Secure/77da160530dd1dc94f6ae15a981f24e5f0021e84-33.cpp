void set_disk_ro(struct gendisk *disk, int flag)
{
	struct disk_part_iter piter;
	struct hd_struct *part;

	if (disk->part0.policy != flag) {
		set_disk_ro_uevent(disk, flag);
		disk->part0.policy = flag;
	}

	disk_part_iter_init(&piter, disk, DISK_PITER_INCL_EMPTY);
	while ((part = disk_part_iter_next(&piter)))
		part->policy = flag;
	disk_part_iter_exit(&piter);
}