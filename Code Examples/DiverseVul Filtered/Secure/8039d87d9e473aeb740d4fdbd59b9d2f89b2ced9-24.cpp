static int btrfs_ioctl_get_features(struct file *file, void __user *arg)
{
	struct btrfs_root *root = BTRFS_I(file_inode(file))->root;
	struct btrfs_super_block *super_block = root->fs_info->super_copy;
	struct btrfs_ioctl_feature_flags features;

	features.compat_flags = btrfs_super_compat_flags(super_block);
	features.compat_ro_flags = btrfs_super_compat_ro_flags(super_block);
	features.incompat_flags = btrfs_super_incompat_flags(super_block);

	if (copy_to_user(arg, &features, sizeof(features)))
		return -EFAULT;

	return 0;
}