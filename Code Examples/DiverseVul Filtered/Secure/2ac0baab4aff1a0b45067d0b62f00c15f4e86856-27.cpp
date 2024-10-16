static int sqfs_frag_lookup(u32 inode_fragment_index,
			    struct squashfs_fragment_block_entry *e)
{
	u64 start, n_blks, src_len, table_offset, start_block;
	unsigned char *metadata_buffer, *metadata, *table;
	struct squashfs_fragment_block_entry *entries;
	struct squashfs_super_block *sblk = ctxt.sblk;
	unsigned long dest_len;
	int block, offset, ret;
	u16 header;

	metadata_buffer = NULL;
	entries = NULL;
	table = NULL;

	if (inode_fragment_index >= get_unaligned_le32(&sblk->fragments))
		return -EINVAL;

	start = get_unaligned_le64(&sblk->fragment_table_start) /
		ctxt.cur_dev->blksz;
	n_blks = sqfs_calc_n_blks(sblk->fragment_table_start,
				  sblk->export_table_start,
				  &table_offset);

	/* Allocate a proper sized buffer to store the fragment index table */
	table = malloc_cache_aligned(n_blks * ctxt.cur_dev->blksz);
	if (!table) {
		ret = -ENOMEM;
		goto out;
	}

	if (sqfs_disk_read(start, n_blks, table) < 0) {
		ret = -EINVAL;
		goto out;
	}

	block = SQFS_FRAGMENT_INDEX(inode_fragment_index);
	offset = SQFS_FRAGMENT_INDEX_OFFSET(inode_fragment_index);

	/*
	 * Get the start offset of the metadata block that contains the right
	 * fragment block entry
	 */
	start_block = get_unaligned_le64(table + table_offset + block *
					 sizeof(u64));

	start = start_block / ctxt.cur_dev->blksz;
	n_blks = sqfs_calc_n_blks(cpu_to_le64(start_block),
				  sblk->fragment_table_start, &table_offset);

	metadata_buffer = malloc_cache_aligned(n_blks * ctxt.cur_dev->blksz);
	if (!metadata_buffer) {
		ret = -ENOMEM;
		goto out;
	}

	if (sqfs_disk_read(start, n_blks, metadata_buffer) < 0) {
		ret = -EINVAL;
		goto out;
	}

	/* Every metadata block starts with a 16-bit header */
	header = get_unaligned_le16(metadata_buffer + table_offset);
	metadata = metadata_buffer + table_offset + SQFS_HEADER_SIZE;

	if (!metadata || !header) {
		ret = -ENOMEM;
		goto out;
	}

	entries = malloc(SQFS_METADATA_BLOCK_SIZE);
	if (!entries) {
		ret = -ENOMEM;
		goto out;
	}

	if (SQFS_COMPRESSED_METADATA(header)) {
		src_len = SQFS_METADATA_SIZE(header);
		dest_len = SQFS_METADATA_BLOCK_SIZE;
		ret = sqfs_decompress(&ctxt, entries, &dest_len, metadata,
				      src_len);
		if (ret) {
			ret = -EINVAL;
			goto out;
		}
	} else {
		memcpy(entries, metadata, SQFS_METADATA_SIZE(header));
	}

	*e = entries[offset];
	ret = SQFS_COMPRESSED_BLOCK(e->size);

out:
	free(entries);
	free(metadata_buffer);
	free(table);

	return ret;
}