void dm_unlock_md_type(struct mapped_device *md)
{
	mutex_unlock(&md->type_lock);
}