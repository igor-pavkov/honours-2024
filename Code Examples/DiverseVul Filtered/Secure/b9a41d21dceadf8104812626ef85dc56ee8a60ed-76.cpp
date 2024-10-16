void dm_put(struct mapped_device *md)
{
	atomic_dec(&md->holders);
}