static int sqfs_read_directory_table(unsigned char **dir_table, u32 **pos_list)
{
	u64 start, n_blks, table_offset, table_size;
	struct squashfs_super_block *sblk = ctxt.sblk;
	int j, ret = 0, metablks_count = -1;
	unsigned char *src_table, *dtb;
	u32 src_len, dest_offset = 0;
	unsigned long dest_len = 0;
	bool compressed;

	*dir_table = NULL;
	*pos_list = NULL;
	/* DIRECTORY TABLE */
	table_size = get_unaligned_le64(&sblk->fragment_table_start) -
		get_unaligned_le64(&sblk->directory_table_start);
	start = get_unaligned_le64(&sblk->directory_table_start) /
		ctxt.cur_dev->blksz;
	n_blks = sqfs_calc_n_blks(sblk->directory_table_start,
				  sblk->fragment_table_start, &table_offset);

	/* Allocate a proper sized buffer (dtb) to store the directory table */
	dtb = malloc_cache_aligned(n_blks * ctxt.cur_dev->blksz);
	if (!dtb)
		return -ENOMEM;

	if (sqfs_disk_read(start, n_blks, dtb) < 0)
		goto out;

	/* Parse directory table (metadata block) header */
	ret = sqfs_read_metablock(dtb, table_offset, &compressed, &src_len);
	if (ret)
		goto out;

	/* Calculate total size to store the whole decompressed table */
	metablks_count = sqfs_count_metablks(dtb, table_offset, table_size);
	if (metablks_count < 1)
		goto out;

	*dir_table = malloc(metablks_count * SQFS_METADATA_BLOCK_SIZE);
	if (!*dir_table)
		goto out;

	*pos_list = malloc(metablks_count * sizeof(u32));
	if (!*pos_list)
		goto out;

	ret = sqfs_get_metablk_pos(*pos_list, dtb, table_offset,
				   metablks_count);
	if (ret) {
		metablks_count = -1;
		goto out;
	}

	src_table = dtb + table_offset + SQFS_HEADER_SIZE;

	/* Extract compressed Directory table */
	dest_offset = 0;
	for (j = 0; j < metablks_count; j++) {
		sqfs_read_metablock(dtb, table_offset, &compressed, &src_len);
		if (compressed) {
			dest_len = SQFS_METADATA_BLOCK_SIZE;
			ret = sqfs_decompress(&ctxt, *dir_table +
					      (j * SQFS_METADATA_BLOCK_SIZE),
					      &dest_len, src_table, src_len);
			if (ret) {
				metablks_count = -1;
				goto out;
			}

			if (dest_len < SQFS_METADATA_BLOCK_SIZE) {
				dest_offset += dest_len;
				break;
			}

			dest_offset += dest_len;
		} else {
			memcpy(*dir_table + (j * SQFS_METADATA_BLOCK_SIZE),
			       src_table, src_len);
		}

		/*
		 * Offsets to the decompression destination, to the metadata
		 * buffer 'dtb' and to the decompression source, respectively.
		 */
		table_offset += src_len + SQFS_HEADER_SIZE;
		src_table += src_len + SQFS_HEADER_SIZE;
	}

out:
	if (metablks_count < 1) {
		free(*dir_table);
		free(*pos_list);
		*dir_table = NULL;
		*pos_list = NULL;
	}
	free(dtb);

	return metablks_count;
}