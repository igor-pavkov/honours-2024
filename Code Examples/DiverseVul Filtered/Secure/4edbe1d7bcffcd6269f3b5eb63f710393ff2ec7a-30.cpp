static struct hash_cell *__get_dev_cell(uint64_t dev)
{
	struct mapped_device *md;
	struct hash_cell *hc;

	md = dm_get_md(huge_decode_dev(dev));
	if (!md)
		return NULL;

	hc = dm_get_mdptr(md);
	if (!hc) {
		dm_put(md);
		return NULL;
	}

	return hc;
}