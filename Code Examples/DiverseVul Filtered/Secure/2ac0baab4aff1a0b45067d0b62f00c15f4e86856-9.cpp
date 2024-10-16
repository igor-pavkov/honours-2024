int sqfs_probe(struct blk_desc *fs_dev_desc, struct disk_partition *fs_partition)
{
	struct squashfs_super_block *sblk;
	int ret;

	ctxt.cur_dev = fs_dev_desc;
	ctxt.cur_part_info = *fs_partition;

	ret = sqfs_read_sblk(&sblk);
	if (ret)
		goto error;

	/* Make sure it has a valid SquashFS magic number*/
	if (get_unaligned_le32(&sblk->s_magic) != SQFS_MAGIC_NUMBER) {
		debug("Bad magic number for SquashFS image.\n");
		ret = -EINVAL;
		goto error;
	}

	ctxt.sblk = sblk;

	ret = sqfs_decompressor_init(&ctxt);
	if (ret) {
		goto error;
	}

	return 0;
error:
	ctxt.cur_dev = NULL;
	free(ctxt.sblk);
	ctxt.sblk = NULL;
	return ret;
}