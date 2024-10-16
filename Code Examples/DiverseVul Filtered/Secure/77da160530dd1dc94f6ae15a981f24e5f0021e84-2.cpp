void disk_part_iter_exit(struct disk_part_iter *piter)
{
	disk_put_part(piter->part);
	piter->part = NULL;
}