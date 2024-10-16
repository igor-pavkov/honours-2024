struct dm_stats *dm_get_stats(struct mapped_device *md)
{
	return &md->stats;
}