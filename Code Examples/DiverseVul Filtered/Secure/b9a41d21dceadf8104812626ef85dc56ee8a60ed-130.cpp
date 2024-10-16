void *dm_get_mdptr(struct mapped_device *md)
{
	return md->interface_ptr;
}