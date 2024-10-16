void dm_destroy_immediate(struct mapped_device *md)
{
	__dm_destroy(md, false);
}