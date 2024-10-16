struct request_queue *dm_get_md_queue(struct mapped_device *md)
{
	return md->queue;
}