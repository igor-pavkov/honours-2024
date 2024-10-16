static int sqfs_read_sblk(struct squashfs_super_block **sblk)
{
	*sblk = malloc_cache_aligned(ctxt.cur_dev->blksz);
	if (!*sblk)
		return -ENOMEM;

	if (sqfs_disk_read(0, 1, *sblk) != 1) {
		free(*sblk);
		sblk = NULL;
		return -EINVAL;
	}

	return 0;
}