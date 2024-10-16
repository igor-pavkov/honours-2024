void sqfs_closedir(struct fs_dir_stream *dirs)
{
	struct squashfs_dir_stream *sqfs_dirs;

	if (!dirs)
		return;

	sqfs_dirs = (struct squashfs_dir_stream *)dirs;
	free(sqfs_dirs->inode_table);
	free(sqfs_dirs->dir_table);
	free(sqfs_dirs->dir_header);
	free(sqfs_dirs);
}