struct mapped_device *dm_get_md(dev_t dev)
{
	struct mapped_device *md;
	unsigned minor = MINOR(dev);

	if (MAJOR(dev) != _major || minor >= (1 << MINORBITS))
		return NULL;

	spin_lock(&_minor_lock);

	md = idr_find(&_minor_idr, minor);
	if (md) {
		if ((md == MINOR_ALLOCED ||
		     (MINOR(disk_devt(dm_disk(md))) != minor) ||
		     dm_deleting_md(md) ||
		     test_bit(DMF_FREEING, &md->flags))) {
			md = NULL;
			goto out;
		}
		dm_get(md);
	}

out:
	spin_unlock(&_minor_lock);

	return md;
}