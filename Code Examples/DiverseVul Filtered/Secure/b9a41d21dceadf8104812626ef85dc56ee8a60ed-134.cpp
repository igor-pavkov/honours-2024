void dm_lock_md_type(struct mapped_device *md)
{
	mutex_lock(&md->type_lock);
}