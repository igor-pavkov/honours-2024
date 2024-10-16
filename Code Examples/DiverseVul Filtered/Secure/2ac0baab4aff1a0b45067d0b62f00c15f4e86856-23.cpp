static int sqfs_search_dir(struct squashfs_dir_stream *dirs, char **token_list,
			   int token_count, u32 *m_list, int m_count)
{
	struct squashfs_super_block *sblk = ctxt.sblk;
	char *path, *target, **sym_tokens, *res, *rem;
	int j, ret = 0, new_inode_number, offset;
	struct squashfs_symlink_inode *sym;
	struct squashfs_ldir_inode *ldir;
	struct squashfs_dir_inode *dir;
	struct fs_dir_stream *dirsp;
	struct fs_dirent *dent;
	unsigned char *table;

	res = NULL;
	rem = NULL;
	path = NULL;
	target = NULL;
	sym_tokens = NULL;

	dirsp = (struct fs_dir_stream *)dirs;

	/* Start by root inode */
	table = sqfs_find_inode(dirs->inode_table, le32_to_cpu(sblk->inodes),
				sblk->inodes, sblk->block_size);

	dir = (struct squashfs_dir_inode *)table;
	ldir = (struct squashfs_ldir_inode *)table;

	/* get directory offset in directory table */
	offset = sqfs_dir_offset(table, m_list, m_count);
	dirs->table = &dirs->dir_table[offset];

	/* Setup directory header */
	dirs->dir_header = malloc(SQFS_DIR_HEADER_SIZE);
	if (!dirs->dir_header)
		return -ENOMEM;

	memcpy(dirs->dir_header, dirs->table, SQFS_DIR_HEADER_SIZE);

	/* Initialize squashfs_dir_stream members */
	dirs->table += SQFS_DIR_HEADER_SIZE;
	dirs->size = get_unaligned_le16(&dir->file_size) - SQFS_DIR_HEADER_SIZE;
	dirs->entry_count = dirs->dir_header->count + 1;

	/* No path given -> root directory */
	if (!strcmp(token_list[0], "/")) {
		dirs->table = &dirs->dir_table[offset];
		memcpy(&dirs->i_dir, dir, sizeof(*dir));
		return 0;
	}

	for (j = 0; j < token_count; j++) {
		if (!sqfs_is_dir(get_unaligned_le16(&dir->inode_type))) {
			printf("** Cannot find directory. **\n");
			ret = -EINVAL;
			goto out;
		}

		while (!sqfs_readdir(dirsp, &dent)) {
			ret = strcmp(dent->name, token_list[j]);
			if (!ret)
				break;
			free(dirs->entry);
			dirs->entry = NULL;
		}

		if (ret) {
			printf("** Cannot find directory. **\n");
			ret = -EINVAL;
			goto out;
		}

		/* Redefine inode as the found token */
		new_inode_number = dirs->entry->inode_offset +
			dirs->dir_header->inode_number;

		/* Get reference to inode in the inode table */
		table = sqfs_find_inode(dirs->inode_table, new_inode_number,
					sblk->inodes, sblk->block_size);
		dir = (struct squashfs_dir_inode *)table;

		/* Check for symbolic link and inode type sanity */
		if (get_unaligned_le16(&dir->inode_type) == SQFS_SYMLINK_TYPE) {
			sym = (struct squashfs_symlink_inode *)table;
			/* Get first j + 1 tokens */
			path = sqfs_concat_tokens(token_list, j + 1);
			if (!path) {
				ret = -ENOMEM;
				goto out;
			}
			/* Resolve for these tokens */
			target = sqfs_resolve_symlink(sym, path);
			if (!target) {
				ret = -ENOMEM;
				goto out;
			}
			/* Join remaining tokens */
			rem = sqfs_concat_tokens(token_list + j + 1, token_count -
						 j - 1);
			if (!rem) {
				ret = -ENOMEM;
				goto out;
			}
			/* Concatenate remaining tokens and symlink's target */
			res = malloc(strlen(rem) + strlen(target) + 1);
			if (!res) {
				ret = -ENOMEM;
				goto out;
			}
			strcpy(res, target);
			res[strlen(target)] = '/';
			strcpy(res + strlen(target) + 1, rem);
			token_count = sqfs_count_tokens(res);

			if (token_count < 0) {
				ret = -EINVAL;
				goto out;
			}

			sym_tokens = malloc(token_count * sizeof(char *));
			if (!sym_tokens) {
				ret = -EINVAL;
				goto out;
			}

			/* Fill tokens list */
			ret = sqfs_tokenize(sym_tokens, token_count, res);
			if (ret) {
				ret = -EINVAL;
				goto out;
			}
			free(dirs->entry);
			dirs->entry = NULL;

			ret = sqfs_search_dir(dirs, sym_tokens, token_count,
					      m_list, m_count);
			goto out;
		} else if (!sqfs_is_dir(get_unaligned_le16(&dir->inode_type))) {
			printf("** Cannot find directory. **\n");
			free(dirs->entry);
			dirs->entry = NULL;
			ret = -EINVAL;
			goto out;
		}

		/* Check if it is an extended dir. */
		if (get_unaligned_le16(&dir->inode_type) == SQFS_LDIR_TYPE)
			ldir = (struct squashfs_ldir_inode *)table;

		/* Get dir. offset into the directory table */
		offset = sqfs_dir_offset(table, m_list, m_count);
		dirs->table = &dirs->dir_table[offset];

		/* Copy directory header */
		memcpy(dirs->dir_header, &dirs->dir_table[offset],
		       SQFS_DIR_HEADER_SIZE);

		/* Check for empty directory */
		if (sqfs_is_empty_dir(table)) {
			printf("Empty directory.\n");
			free(dirs->entry);
			dirs->entry = NULL;
			ret = SQFS_EMPTY_DIR;
			goto out;
		}

		dirs->table += SQFS_DIR_HEADER_SIZE;
		dirs->size = get_unaligned_le16(&dir->file_size);
		dirs->entry_count = dirs->dir_header->count + 1;
		dirs->size -= SQFS_DIR_HEADER_SIZE;
		free(dirs->entry);
		dirs->entry = NULL;
	}

	offset = sqfs_dir_offset(table, m_list, m_count);
	dirs->table = &dirs->dir_table[offset];

	if (get_unaligned_le16(&dir->inode_type) == SQFS_DIR_TYPE)
		memcpy(&dirs->i_dir, dir, sizeof(*dir));
	else
		memcpy(&dirs->i_ldir, ldir, sizeof(*ldir));

out:
	free(res);
	free(rem);
	free(path);
	free(target);
	free(sym_tokens);
	return ret;
}