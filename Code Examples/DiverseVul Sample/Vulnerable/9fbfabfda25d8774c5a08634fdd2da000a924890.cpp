static struct block_device *add_partition(struct gendisk *disk, int partno,
				sector_t start, sector_t len, int flags,
				struct partition_meta_info *info)
{
	dev_t devt = MKDEV(0, 0);
	struct device *ddev = disk_to_dev(disk);
	struct device *pdev;
	struct block_device *bdev;
	const char *dname;
	int err;

	lockdep_assert_held(&disk->open_mutex);

	if (partno >= disk_max_parts(disk))
		return ERR_PTR(-EINVAL);

	/*
	 * Partitions are not supported on zoned block devices that are used as
	 * such.
	 */
	switch (disk->queue->limits.zoned) {
	case BLK_ZONED_HM:
		pr_warn("%s: partitions not supported on host managed zoned block device\n",
			disk->disk_name);
		return ERR_PTR(-ENXIO);
	case BLK_ZONED_HA:
		pr_info("%s: disabling host aware zoned block device support due to partitions\n",
			disk->disk_name);
		blk_queue_set_zoned(disk, BLK_ZONED_NONE);
		break;
	case BLK_ZONED_NONE:
		break;
	}

	if (xa_load(&disk->part_tbl, partno))
		return ERR_PTR(-EBUSY);

	/* ensure we always have a reference to the whole disk */
	get_device(disk_to_dev(disk));

	err = -ENOMEM;
	bdev = bdev_alloc(disk, partno);
	if (!bdev)
		goto out_put_disk;

	bdev->bd_start_sect = start;
	bdev_set_nr_sectors(bdev, len);

	pdev = &bdev->bd_device;
	dname = dev_name(ddev);
	if (isdigit(dname[strlen(dname) - 1]))
		dev_set_name(pdev, "%sp%d", dname, partno);
	else
		dev_set_name(pdev, "%s%d", dname, partno);

	device_initialize(pdev);
	pdev->class = &block_class;
	pdev->type = &part_type;
	pdev->parent = ddev;

	/* in consecutive minor range? */
	if (bdev->bd_partno < disk->minors) {
		devt = MKDEV(disk->major, disk->first_minor + bdev->bd_partno);
	} else {
		err = blk_alloc_ext_minor();
		if (err < 0)
			goto out_put;
		devt = MKDEV(BLOCK_EXT_MAJOR, err);
	}
	pdev->devt = devt;

	if (info) {
		err = -ENOMEM;
		bdev->bd_meta_info = kmemdup(info, sizeof(*info), GFP_KERNEL);
		if (!bdev->bd_meta_info)
			goto out_put;
	}

	/* delay uevent until 'holders' subdir is created */
	dev_set_uevent_suppress(pdev, 1);
	err = device_add(pdev);
	if (err)
		goto out_put;

	err = -ENOMEM;
	bdev->bd_holder_dir = kobject_create_and_add("holders", &pdev->kobj);
	if (!bdev->bd_holder_dir)
		goto out_del;

	dev_set_uevent_suppress(pdev, 0);
	if (flags & ADDPART_FLAG_WHOLEDISK) {
		err = device_create_file(pdev, &dev_attr_whole_disk);
		if (err)
			goto out_del;
	}

	/* everything is up and running, commence */
	err = xa_insert(&disk->part_tbl, partno, bdev, GFP_KERNEL);
	if (err)
		goto out_del;
	bdev_add(bdev, devt);

	/* suppress uevent if the disk suppresses it */
	if (!dev_get_uevent_suppress(ddev))
		kobject_uevent(&pdev->kobj, KOBJ_ADD);
	return bdev;

out_del:
	kobject_put(bdev->bd_holder_dir);
	device_del(pdev);
out_put:
	put_device(pdev);
out_put_disk:
	put_disk(disk);
	return ERR_PTR(err);
}