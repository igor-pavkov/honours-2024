void dm_put_table_device(struct mapped_device *md, struct dm_dev *d)
{
	struct table_device *td = container_of(d, struct table_device, dm_dev);

	mutex_lock(&md->table_devices_lock);
	if (refcount_dec_and_test(&td->count)) {
		close_table_device(td, md);
		list_del(&td->list);
		kfree(td);
	}
	mutex_unlock(&md->table_devices_lock);
}