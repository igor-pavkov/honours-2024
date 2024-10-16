int sqfs_size(const char *filename, loff_t *size)
{
	struct squashfs_super_block *sblk = ctxt.sblk;
	struct squashfs_symlink_inode *symlink;
	struct fs_dir_stream *dirsp = NULL;
	struct squashfs_base_inode *base;
	struct squashfs_dir_stream *dirs;
	struct squashfs_lreg_inode *lreg;
	struct squashfs_reg_inode *reg;
	char *dir, *file, *resolved;
	struct fs_dirent *dent;
	unsigned char *ipos;
	int ret, i_number;

	sqfs_split_path(&file, &dir, filename);
	/*
	 * sqfs_opendir will uncompress inode and directory tables, and will
	 * return a pointer to the directory that contains the requested file.
	 */
	ret = sqfs_opendir(dir, &dirsp);
	if (ret) {
		ret = -EINVAL;
		goto free_strings;
	}

	dirs = (struct squashfs_dir_stream *)dirsp;

	while (!sqfs_readdir(dirsp, &dent)) {
		ret = strcmp(dent->name, file);
		if (!ret)
			break;
		free(dirs->entry);
		dirs->entry = NULL;
	}

	if (ret) {
		printf("File not found.\n");
		*size = 0;
		ret = -EINVAL;
		goto free_strings;
	}

	i_number = dirs->dir_header->inode_number + dirs->entry->inode_offset;
	ipos = sqfs_find_inode(dirs->inode_table, i_number, sblk->inodes,
			       sblk->block_size);
	free(dirs->entry);
	dirs->entry = NULL;

	base = (struct squashfs_base_inode *)ipos;
	switch (get_unaligned_le16(&base->inode_type)) {
	case SQFS_REG_TYPE:
		reg = (struct squashfs_reg_inode *)ipos;
		*size = get_unaligned_le32(&reg->file_size);
		break;
	case SQFS_LREG_TYPE:
		lreg = (struct squashfs_lreg_inode *)ipos;
		*size = get_unaligned_le64(&lreg->file_size);
		break;
	case SQFS_SYMLINK_TYPE:
	case SQFS_LSYMLINK_TYPE:
		symlink = (struct squashfs_symlink_inode *)ipos;
		resolved = sqfs_resolve_symlink(symlink, filename);
		ret = sqfs_size(resolved, size);
		free(resolved);
		break;
	case SQFS_BLKDEV_TYPE:
	case SQFS_CHRDEV_TYPE:
	case SQFS_LBLKDEV_TYPE:
	case SQFS_LCHRDEV_TYPE:
	case SQFS_FIFO_TYPE:
	case SQFS_SOCKET_TYPE:
	case SQFS_LFIFO_TYPE:
	case SQFS_LSOCKET_TYPE:
	default:
		printf("Unable to recover entry's size.\n");
		*size = 0;
		ret = -EINVAL;
		break;
	}

free_strings:
	free(dir);
	free(file);

	sqfs_closedir(dirsp);

	return ret;
}