void disable_write_same(struct mapped_device *md)
{
	struct queue_limits *limits = dm_get_queue_limits(md);

	/* device doesn't really support WRITE SAME, disable it */
	limits->max_write_same_sectors = 0;
}