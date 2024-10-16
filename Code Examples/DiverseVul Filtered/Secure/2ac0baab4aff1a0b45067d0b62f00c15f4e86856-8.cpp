int sqfs_exists(const char *filename)
{
	struct fs_dir_stream *dirsp = NULL;
	struct squashfs_dir_stream *dirs;
	char *dir, *file;
	struct fs_dirent *dent;
	int ret;

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

	sqfs_closedir(dirsp);

free_strings:
	free(dir);
	free(file);

	return ret == 0;
}