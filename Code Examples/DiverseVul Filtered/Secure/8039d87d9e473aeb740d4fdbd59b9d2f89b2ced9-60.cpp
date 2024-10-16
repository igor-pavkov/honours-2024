static long btrfs_ioctl_quota_ctl(struct file *file, void __user *arg)
{
	struct btrfs_root *root = BTRFS_I(file_inode(file))->root;
	struct btrfs_ioctl_quota_ctl_args *sa;
	struct btrfs_trans_handle *trans = NULL;
	int ret;
	int err;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	ret = mnt_want_write_file(file);
	if (ret)
		return ret;

	sa = memdup_user(arg, sizeof(*sa));
	if (IS_ERR(sa)) {
		ret = PTR_ERR(sa);
		goto drop_write;
	}

	down_write(&root->fs_info->subvol_sem);
	trans = btrfs_start_transaction(root->fs_info->tree_root, 2);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		goto out;
	}

	switch (sa->cmd) {
	case BTRFS_QUOTA_CTL_ENABLE:
		ret = btrfs_quota_enable(trans, root->fs_info);
		break;
	case BTRFS_QUOTA_CTL_DISABLE:
		ret = btrfs_quota_disable(trans, root->fs_info);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	err = btrfs_commit_transaction(trans, root->fs_info->tree_root);
	if (err && !ret)
		ret = err;
out:
	kfree(sa);
	up_write(&root->fs_info->subvol_sem);
drop_write:
	mnt_drop_write_file(file);
	return ret;
}