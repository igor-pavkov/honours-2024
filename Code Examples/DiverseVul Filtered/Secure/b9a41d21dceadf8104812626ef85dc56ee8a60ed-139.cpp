enum dm_queue_mode dm_get_md_type(struct mapped_device *md)
{
	return md->type;
}