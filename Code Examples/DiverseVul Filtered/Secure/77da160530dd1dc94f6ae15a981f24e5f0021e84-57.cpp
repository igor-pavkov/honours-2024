void blk_free_devt(dev_t devt)
{
	if (devt == MKDEV(0, 0))
		return;

	if (MAJOR(devt) == BLOCK_EXT_MAJOR) {
		spin_lock_bh(&ext_devt_lock);
		idr_remove(&ext_devt_idr, blk_mangle_minor(MINOR(devt)));
		spin_unlock_bh(&ext_devt_lock);
	}
}