static void floppy_probe(dev_t dev)
{
	unsigned int drive = (MINOR(dev) & 3) | ((MINOR(dev) & 0x80) >> 5);
	unsigned int type = (MINOR(dev) >> 2) & 0x1f;

	if (drive >= N_DRIVE || !floppy_available(drive) ||
	    type >= ARRAY_SIZE(floppy_type))
		return;

	mutex_lock(&floppy_probe_lock);
	if (disks[drive][type])
		goto out;
	if (floppy_alloc_disk(drive, type))
		goto out;
	if (add_disk(disks[drive][type]))
		goto cleanup_disk;
out:
	mutex_unlock(&floppy_probe_lock);
	return;

cleanup_disk:
	blk_cleanup_disk(disks[drive][type]);
	disks[drive][type] = NULL;
	mutex_unlock(&floppy_probe_lock);
}