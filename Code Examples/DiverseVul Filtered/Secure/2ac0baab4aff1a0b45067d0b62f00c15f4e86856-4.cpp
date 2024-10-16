static int sqfs_disk_read(__u32 block, __u32 nr_blocks, void *buf)
{
	ulong ret;

	if (!ctxt.cur_dev)
		return -1;

	ret = blk_dread(ctxt.cur_dev, ctxt.cur_part_info.start + block,
			nr_blocks, buf);

	if (ret != nr_blocks)
		return -1;

	return ret;
}