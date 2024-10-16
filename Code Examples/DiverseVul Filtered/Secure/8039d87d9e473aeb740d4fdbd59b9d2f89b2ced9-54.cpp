static int btrfs_ioctl_get_fslabel(struct file *file, void __user *arg)
{
	struct btrfs_root *root = BTRFS_I(file_inode(file))->root;
	size_t len;
	int ret;
	char label[BTRFS_LABEL_SIZE];

	spin_lock(&root->fs_info->super_lock);
	memcpy(label, root->fs_info->super_copy->label, BTRFS_LABEL_SIZE);
	spin_unlock(&root->fs_info->super_lock);

	len = strnlen(label, BTRFS_LABEL_SIZE);

	if (len == BTRFS_LABEL_SIZE) {
		btrfs_warn(root->fs_info,
			"label is too long, return the first %zu bytes", --len);
	}

	ret = copy_to_user(arg, label, len);

	return ret ? -EFAULT : 0;
}