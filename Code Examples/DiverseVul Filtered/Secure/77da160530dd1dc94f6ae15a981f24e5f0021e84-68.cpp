void put_disk(struct gendisk *disk)
{
	if (disk)
		kobject_put(&disk_to_dev(disk)->kobj);
}