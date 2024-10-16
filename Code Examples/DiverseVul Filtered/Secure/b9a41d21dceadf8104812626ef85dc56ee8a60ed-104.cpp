static struct table_device *find_table_device(struct list_head *l, dev_t dev,
					      fmode_t mode) {
	struct table_device *td;

	list_for_each_entry(td, l, list)
		if (td->dm_dev.bdev->bd_dev == dev && td->dm_dev.mode == mode)
			return td;

	return NULL;
}