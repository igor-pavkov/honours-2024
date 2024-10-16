static long btrfs_ioctl_quota_rescan(struct file *file, void __user *arg)
{
	struct btrfs_root *root = BTRFS_I(file_inode(file))->root;
	struct btrfs_ioctl_quota_rescan_args *qsa;
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	ret = mnt_want_write_file(file);
	if (ret)
		return ret;

	qsa = memdup_user(arg, sizeof(*qsa));
	if (IS_ERR(qsa)) {
		ret = PTR_ERR(qsa);
		goto drop_write;
	}

	if (qsa->flags) {
		ret = -EINVAL;
		goto out;
	}

	ret = btrfs_qgroup_rescan(root->fs_info);

out:
	kfree(qsa);
drop_write:
	mnt_drop_write_file(file);
	return ret;
}