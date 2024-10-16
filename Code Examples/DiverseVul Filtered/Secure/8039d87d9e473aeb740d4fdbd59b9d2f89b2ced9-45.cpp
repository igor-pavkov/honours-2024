static long btrfs_ioctl_quota_rescan_wait(struct file *file, void __user *arg)
{
	struct btrfs_root *root = BTRFS_I(file_inode(file))->root;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	return btrfs_qgroup_wait_for_completion(root->fs_info);
}