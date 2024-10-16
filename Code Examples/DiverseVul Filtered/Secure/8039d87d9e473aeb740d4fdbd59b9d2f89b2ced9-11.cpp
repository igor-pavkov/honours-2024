static long btrfs_ioctl_trans_start(struct file *file)
{
	struct inode *inode = file_inode(file);
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_trans_handle *trans;
	int ret;

	ret = -EPERM;
	if (!capable(CAP_SYS_ADMIN))
		goto out;

	ret = -EINPROGRESS;
	if (file->private_data)
		goto out;

	ret = -EROFS;
	if (btrfs_root_readonly(root))
		goto out;

	ret = mnt_want_write_file(file);
	if (ret)
		goto out;

	atomic_inc(&root->fs_info->open_ioctl_trans);

	ret = -ENOMEM;
	trans = btrfs_start_ioctl_transaction(root);
	if (IS_ERR(trans))
		goto out_drop;

	file->private_data = trans;
	return 0;

out_drop:
	atomic_dec(&root->fs_info->open_ioctl_trans);
	mnt_drop_write_file(file);
out:
	return ret;
}