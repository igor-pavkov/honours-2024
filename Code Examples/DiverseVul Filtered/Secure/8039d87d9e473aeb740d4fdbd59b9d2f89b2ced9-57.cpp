long btrfs_ioctl_trans_end(struct file *file)
{
	struct inode *inode = file_inode(file);
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_trans_handle *trans;

	trans = file->private_data;
	if (!trans)
		return -EINVAL;
	file->private_data = NULL;

	btrfs_end_transaction(trans, root);

	atomic_dec(&root->fs_info->open_ioctl_trans);

	mnt_drop_write_file(file);
	return 0;
}