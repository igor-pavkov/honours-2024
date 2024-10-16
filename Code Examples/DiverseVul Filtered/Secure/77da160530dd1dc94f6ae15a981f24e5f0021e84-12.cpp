void del_gendisk(struct gendisk *disk)
{
	struct disk_part_iter piter;
	struct hd_struct *part;

	blk_integrity_del(disk);
	disk_del_events(disk);

	/* invalidate stuff */
	disk_part_iter_init(&piter, disk,
			     DISK_PITER_INCL_EMPTY | DISK_PITER_REVERSE);
	while ((part = disk_part_iter_next(&piter))) {
		invalidate_partition(disk, part->partno);
		delete_partition(disk, part->partno);
	}
	disk_part_iter_exit(&piter);

	invalidate_partition(disk, 0);
	set_capacity(disk, 0);
	disk->flags &= ~GENHD_FL_UP;

	sysfs_remove_link(&disk_to_dev(disk)->kobj, "bdi");
	blk_unregister_queue(disk);
	blk_unregister_region(disk_devt(disk), disk->minors);

	part_stat_set_all(&disk->part0, 0);
	disk->part0.stamp = 0;

	kobject_put(disk->part0.holder_dir);
	kobject_put(disk->slave_dir);
	if (!sysfs_deprecated)
		sysfs_remove_link(block_depr, dev_name(disk_to_dev(disk)));
	pm_runtime_set_memalloc_noio(disk_to_dev(disk), false);
	device_del(disk_to_dev(disk));
}