int sqfs_read(const char *filename, void *buf, loff_t offset, loff_t len,
	      loff_t *actread)
{
	char *dir = NULL, *fragment_block, *datablock = NULL;
	char *fragment = NULL, *file = NULL, *resolved, *data;
	u64 start, n_blks, table_size, data_offset, table_offset, sparse_size;
	int ret, j, i_number, datablk_count = 0;
	struct squashfs_super_block *sblk = ctxt.sblk;
	struct squashfs_fragment_block_entry frag_entry;
	struct squashfs_file_info finfo = {0};
	struct squashfs_symlink_inode *symlink;
	struct fs_dir_stream *dirsp = NULL;
	struct squashfs_dir_stream *dirs;
	struct squashfs_lreg_inode *lreg;
	struct squashfs_base_inode *base;
	struct squashfs_reg_inode *reg;
	unsigned long dest_len;
	struct fs_dirent *dent;
	unsigned char *ipos;

	*actread = 0;

	if (offset) {
		/*
		 * TODO: implement reading at an offset in file
		 */
		printf("Error: reading at a specific offset in a squashfs file is not supported yet.\n");
		return -EINVAL;
	}

	/*
	 * sqfs_opendir will uncompress inode and directory tables, and will
	 * return a pointer to the directory that contains the requested file.
	 */
	sqfs_split_path(&file, &dir, filename);
	ret = sqfs_opendir(dir, &dirsp);
	if (ret) {
		goto out;
	}

	dirs = (struct squashfs_dir_stream *)dirsp;

	/* For now, only regular files are able to be loaded */
	while (!sqfs_readdir(dirsp, &dent)) {
		ret = strcmp(dent->name, file);
		if (!ret)
			break;

		free(dirs->entry);
		dirs->entry = NULL;
	}

	if (ret) {
		printf("File not found.\n");
		*actread = 0;
		ret = -ENOENT;
		goto out;
	}

	i_number = dirs->dir_header->inode_number + dirs->entry->inode_offset;
	ipos = sqfs_find_inode(dirs->inode_table, i_number, sblk->inodes,
			       sblk->block_size);

	base = (struct squashfs_base_inode *)ipos;
	switch (get_unaligned_le16(&base->inode_type)) {
	case SQFS_REG_TYPE:
		reg = (struct squashfs_reg_inode *)ipos;
		datablk_count = sqfs_get_regfile_info(reg, &finfo, &frag_entry,
						      sblk->block_size);
		if (datablk_count < 0) {
			ret = -EINVAL;
			goto out;
		}

		memcpy(finfo.blk_sizes, ipos + sizeof(*reg),
		       datablk_count * sizeof(u32));
		break;
	case SQFS_LREG_TYPE:
		lreg = (struct squashfs_lreg_inode *)ipos;
		datablk_count = sqfs_get_lregfile_info(lreg, &finfo,
						       &frag_entry,
						       sblk->block_size);
		if (datablk_count < 0) {
			ret = -EINVAL;
			goto out;
		}

		memcpy(finfo.blk_sizes, ipos + sizeof(*lreg),
		       datablk_count * sizeof(u32));
		break;
	case SQFS_SYMLINK_TYPE:
	case SQFS_LSYMLINK_TYPE:
		symlink = (struct squashfs_symlink_inode *)ipos;
		resolved = sqfs_resolve_symlink(symlink, filename);
		ret = sqfs_read(resolved, buf, offset, len, actread);
		free(resolved);
		goto out;
	case SQFS_BLKDEV_TYPE:
	case SQFS_CHRDEV_TYPE:
	case SQFS_LBLKDEV_TYPE:
	case SQFS_LCHRDEV_TYPE:
	case SQFS_FIFO_TYPE:
	case SQFS_SOCKET_TYPE:
	case SQFS_LFIFO_TYPE:
	case SQFS_LSOCKET_TYPE:
	default:
		printf("Unsupported entry type\n");
		ret = -EINVAL;
		goto out;
	}

	/* If the user specifies a length, check its sanity */
	if (len) {
		if (len > finfo.size) {
			ret = -EINVAL;
			goto out;
		}

		finfo.size = len;
	} else {
		len = finfo.size;
	}

	if (datablk_count) {
		data_offset = finfo.start;
		datablock = malloc(get_unaligned_le32(&sblk->block_size));
		if (!datablock) {
			ret = -ENOMEM;
			goto out;
		}
	}

	for (j = 0; j < datablk_count; j++) {
		char *data_buffer;

		start = lldiv(data_offset, ctxt.cur_dev->blksz);
		table_size = SQFS_BLOCK_SIZE(finfo.blk_sizes[j]);
		table_offset = data_offset - (start * ctxt.cur_dev->blksz);
		n_blks = DIV_ROUND_UP(table_size + table_offset,
				      ctxt.cur_dev->blksz);

		/* Don't load any data for sparse blocks */
		if (finfo.blk_sizes[j] == 0) {
			n_blks = 0;
			table_offset = 0;
			data_buffer = NULL;
			data = NULL;
		} else {
			data_buffer = malloc_cache_aligned(n_blks * ctxt.cur_dev->blksz);

			if (!data_buffer) {
				ret = -ENOMEM;
				goto out;
			}

			ret = sqfs_disk_read(start, n_blks, data_buffer);
			if (ret < 0) {
				/*
				 * Possible causes: too many data blocks or too large
				 * SquashFS block size. Tip: re-compile the SquashFS
				 * image with mksquashfs's -b <block_size> option.
				 */
				printf("Error: too many data blocks to be read.\n");
				goto out;
			}

			data = data_buffer + table_offset;
		}

		/* Load the data */
		if (finfo.blk_sizes[j] == 0) {
			/* This is a sparse block */
			sparse_size = get_unaligned_le32(&sblk->block_size);
			if ((*actread + sparse_size) > len)
				sparse_size = len - *actread;
			memset(buf + *actread, 0, sparse_size);
			*actread += sparse_size;
		} else if (SQFS_COMPRESSED_BLOCK(finfo.blk_sizes[j])) {
			dest_len = get_unaligned_le32(&sblk->block_size);
			ret = sqfs_decompress(&ctxt, datablock, &dest_len,
					      data, table_size);
			if (ret)
				goto out;

			if ((*actread + dest_len) > len)
				dest_len = len - *actread;
			memcpy(buf + *actread, datablock, dest_len);
			*actread += dest_len;
		} else {
			if ((*actread + table_size) > len)
				table_size = len - *actread;
			memcpy(buf + *actread, data, table_size);
			*actread += table_size;
		}

		data_offset += table_size;
		free(data_buffer);
		if (*actread >= len)
			break;
	}

	/*
	 * There is no need to continue if the file is not fragmented.
	 */
	if (!finfo.frag) {
		ret = 0;
		goto out;
	}

	start = lldiv(frag_entry.start, ctxt.cur_dev->blksz);
	table_size = SQFS_BLOCK_SIZE(frag_entry.size);
	table_offset = frag_entry.start - (start * ctxt.cur_dev->blksz);
	n_blks = DIV_ROUND_UP(table_size + table_offset, ctxt.cur_dev->blksz);

	fragment = malloc_cache_aligned(n_blks * ctxt.cur_dev->blksz);

	if (!fragment) {
		ret = -ENOMEM;
		goto out;
	}

	ret = sqfs_disk_read(start, n_blks, fragment);
	if (ret < 0)
		goto out;

	/* File compressed and fragmented */
	if (finfo.frag && finfo.comp) {
		dest_len = get_unaligned_le32(&sblk->block_size);
		fragment_block = malloc(dest_len);
		if (!fragment_block) {
			ret = -ENOMEM;
			goto out;
		}

		ret = sqfs_decompress(&ctxt, fragment_block, &dest_len,
				      (void *)fragment  + table_offset,
				      frag_entry.size);
		if (ret) {
			free(fragment_block);
			goto out;
		}

		memcpy(buf + *actread, &fragment_block[finfo.offset], finfo.size - *actread);
		*actread = finfo.size;

		free(fragment_block);

	} else if (finfo.frag && !finfo.comp) {
		fragment_block = (void *)fragment + table_offset;

		memcpy(buf + *actread, &fragment_block[finfo.offset], finfo.size - *actread);
		*actread = finfo.size;
	}

out:
	free(fragment);
	free(datablock);
	free(file);
	free(dir);
	free(finfo.blk_sizes);
	sqfs_closedir(dirsp);

	return ret;
}