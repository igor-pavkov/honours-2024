int blk_alloc_devt(struct hd_struct *part, dev_t *devt)
{
	struct gendisk *disk = part_to_disk(part);
	int idx;

	/* in consecutive minor range? */
	if (part->partno < disk->minors) {
		*devt = MKDEV(disk->major, disk->first_minor + part->partno);
		return 0;
	}

	/* allocate ext devt */
	idr_preload(GFP_KERNEL);

	spin_lock_bh(&ext_devt_lock);
	idx = idr_alloc(&ext_devt_idr, part, 0, NR_EXT_DEVT, GFP_NOWAIT);
	spin_unlock_bh(&ext_devt_lock);

	idr_preload_end();
	if (idx < 0)
		return idx == -ENOSPC ? -EBUSY : idx;

	*devt = MKDEV(BLOCK_EXT_MAJOR, blk_mangle_minor(idx));
	return 0;
}