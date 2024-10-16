static size_t dm_dax_copy_from_iter(struct dax_device *dax_dev, pgoff_t pgoff,
		void *addr, size_t bytes, struct iov_iter *i)
{
	struct mapped_device *md = dax_get_private(dax_dev);
	sector_t sector = pgoff * PAGE_SECTORS;
	struct dm_target *ti;
	long ret = 0;
	int srcu_idx;

	ti = dm_dax_get_live_target(md, sector, &srcu_idx);

	if (!ti)
		goto out;
	if (!ti->type->dax_copy_from_iter) {
		ret = copy_from_iter(addr, bytes, i);
		goto out;
	}
	ret = ti->type->dax_copy_from_iter(ti, pgoff, addr, bytes, i);
 out:
	dm_put_live_table(md, srcu_idx);

	return ret;
}