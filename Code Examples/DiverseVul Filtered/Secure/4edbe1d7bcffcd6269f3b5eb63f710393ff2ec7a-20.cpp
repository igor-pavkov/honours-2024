static struct mapped_device *find_device(struct dm_ioctl *param)
{
	struct hash_cell *hc;
	struct mapped_device *md = NULL;

	down_read(&_hash_lock);
	hc = __find_device_hash_cell(param);
	if (hc)
		md = hc->md;
	up_read(&_hash_lock);

	return md;
}