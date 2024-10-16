static int sqfs_read_inode_table(unsigned char **inode_table)
{
	struct squashfs_super_block *sblk = ctxt.sblk;
	u64 start, n_blks, table_offset, table_size;
	int j, ret = 0, metablks_count;
	unsigned char *src_table, *itb;
	u32 src_len, dest_offset = 0;
	unsigned long dest_len = 0;
	bool compressed;

	table_size = get_unaligned_le64(&sblk->directory_table_start) -
		get_unaligned_le64(&sblk->inode_table_start);
	start = get_unaligned_le64(&sblk->inode_table_start) /
		ctxt.cur_dev->blksz;
	n_blks = sqfs_calc_n_blks(sblk->inode_table_start,
				  sblk->directory_table_start, &table_offset);

	/* Allocate a proper sized buffer (itb) to store the inode table */
	itb = malloc_cache_aligned(n_blks * ctxt.cur_dev->blksz);
	if (!itb)
		return -ENOMEM;

	if (sqfs_disk_read(start, n_blks, itb) < 0) {
		ret = -EINVAL;
		goto free_itb;
	}

	/* Parse inode table (metadata block) header */
	ret = sqfs_read_metablock(itb, table_offset, &compressed, &src_len);
	if (ret) {
		ret = -EINVAL;
		goto free_itb;
	}

	/* Calculate size to store the whole decompressed table */
	metablks_count = sqfs_count_metablks(itb, table_offset, table_size);
	if (metablks_count < 1) {
		ret = -EINVAL;
		goto free_itb;
	}

	*inode_table = malloc(metablks_count * SQFS_METADATA_BLOCK_SIZE);
	if (!*inode_table) {
		ret = -ENOMEM;
		printf("Error: failed to allocate squashfs inode_table of size %i, increasing CONFIG_SYS_MALLOC_LEN could help\n",
		       metablks_count * SQFS_METADATA_BLOCK_SIZE);
		goto free_itb;
	}

	src_table = itb + table_offset + SQFS_HEADER_SIZE;

	/* Extract compressed Inode table */
	for (j = 0; j < metablks_count; j++) {
		sqfs_read_metablock(itb, table_offset, &compressed, &src_len);
		if (compressed) {
			dest_len = SQFS_METADATA_BLOCK_SIZE;
			ret = sqfs_decompress(&ctxt, *inode_table +
					      dest_offset, &dest_len,
					      src_table, src_len);
			if (ret) {
				free(*inode_table);
				*inode_table = NULL;
				goto free_itb;
			}

			dest_offset += dest_len;
		} else {
			memcpy(*inode_table + (j * SQFS_METADATA_BLOCK_SIZE),
			       src_table, src_len);
		}

		/*
		 * Offsets to the decompression destination, to the metadata
		 * buffer 'itb' and to the decompression source, respectively.
		 */

		table_offset += src_len + SQFS_HEADER_SIZE;
		src_table += src_len + SQFS_HEADER_SIZE;
	}

free_itb:
	free(itb);

	return ret;
}