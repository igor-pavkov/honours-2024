struct gendisk *dm_disk(struct mapped_device *md)
{
	return md->disk;
}