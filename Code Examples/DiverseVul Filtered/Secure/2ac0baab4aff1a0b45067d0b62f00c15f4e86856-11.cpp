static int sqfs_get_regfile_info(struct squashfs_reg_inode *reg,
				 struct squashfs_file_info *finfo,
				 struct squashfs_fragment_block_entry *fentry,
				 __le32 blksz)
{
	int datablk_count = 0, ret;

	finfo->size = get_unaligned_le32(&reg->file_size);
	finfo->offset = get_unaligned_le32(&reg->offset);
	finfo->start = get_unaligned_le32(&reg->start_block);
	finfo->frag = SQFS_IS_FRAGMENTED(get_unaligned_le32(&reg->fragment));

	if (finfo->frag && finfo->offset == 0xFFFFFFFF)
		return -EINVAL;

	if (finfo->size < 1 || finfo->start == 0xFFFFFFFF)
		return -EINVAL;

	if (finfo->frag) {
		datablk_count = finfo->size / le32_to_cpu(blksz);
		ret = sqfs_frag_lookup(get_unaligned_le32(&reg->fragment),
				       fentry);
		if (ret < 0)
			return -EINVAL;
		finfo->comp = ret;
		if (fentry->size < 1 || fentry->start == 0x7FFFFFFF)
			return -EINVAL;
	} else {
		datablk_count = DIV_ROUND_UP(finfo->size, le32_to_cpu(blksz));
	}

	finfo->blk_sizes = malloc(datablk_count * sizeof(u32));
	if (!finfo->blk_sizes)
		return -ENOMEM;

	return datablk_count;
}