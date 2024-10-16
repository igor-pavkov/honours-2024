int sqfs_opendir(const char *filename, struct fs_dir_stream **dirsp)
{
	unsigned char *inode_table = NULL, *dir_table = NULL;
	int j, token_count = 0, ret = 0, metablks_count;
	struct squashfs_dir_stream *dirs;
	char **token_list = NULL, *path = NULL;
	u32 *pos_list = NULL;

	dirs = calloc(1, sizeof(*dirs));
	if (!dirs)
		return -EINVAL;

	/* these should be set to NULL to prevent dangling pointers */
	dirs->dir_header = NULL;
	dirs->entry = NULL;
	dirs->table = NULL;
	dirs->inode_table = NULL;
	dirs->dir_table = NULL;

	ret = sqfs_read_inode_table(&inode_table);
	if (ret) {
		ret = -EINVAL;
		goto out;
	}

	metablks_count = sqfs_read_directory_table(&dir_table, &pos_list);
	if (metablks_count < 1) {
		ret = -EINVAL;
		goto out;
	}

	/* Tokenize filename */
	token_count = sqfs_count_tokens(filename);
	if (token_count < 0) {
		ret = -EINVAL;
		goto out;
	}

	path = strdup(filename);
	if (!path) {
		ret = -EINVAL;
		goto out;
	}

	token_list = malloc(token_count * sizeof(char *));
	if (!token_list) {
		ret = -EINVAL;
		goto out;
	}

	/* Fill tokens list */
	ret = sqfs_tokenize(token_list, token_count, path);
	if (ret)
		goto out;
	/*
	 * ldir's (extended directory) size is greater than dir, so it works as
	 * a general solution for the malloc size, since 'i' is a union.
	 */
	dirs->inode_table = inode_table;
	dirs->dir_table = dir_table;
	ret = sqfs_search_dir(dirs, token_list, token_count, pos_list,
			      metablks_count);
	if (ret)
		goto out;

	if (le16_to_cpu(dirs->i_dir.inode_type) == SQFS_DIR_TYPE)
		dirs->size = le16_to_cpu(dirs->i_dir.file_size);
	else
		dirs->size = le32_to_cpu(dirs->i_ldir.file_size);

	/* Setup directory header */
	memcpy(dirs->dir_header, dirs->table, SQFS_DIR_HEADER_SIZE);
	dirs->entry_count = dirs->dir_header->count + 1;
	dirs->size -= SQFS_DIR_HEADER_SIZE;

	/* Setup entry */
	dirs->entry = NULL;
	dirs->table += SQFS_DIR_HEADER_SIZE;

	*dirsp = (struct fs_dir_stream *)dirs;

out:
	for (j = 0; j < token_count; j++)
		free(token_list[j]);
	free(token_list);
	free(pos_list);
	free(path);
	if (ret) {
		free(inode_table);
		free(dirs);
	}

	return ret;
}