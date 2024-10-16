static long btrfs_ioctl_quota_rescan_status(struct file *file, void __user *arg)
{
	struct btrfs_root *root = BTRFS_I(file_inode(file))->root;
	struct btrfs_ioctl_quota_rescan_args *qsa;
	int ret = 0;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	qsa = kzalloc(sizeof(*qsa), GFP_NOFS);
	if (!qsa)
		return -ENOMEM;

	if (root->fs_info->qgroup_flags & BTRFS_QGROUP_STATUS_FLAG_RESCAN) {
		qsa->flags = 1;
		qsa->progress = root->fs_info->qgroup_rescan_progress.objectid;
	}

	if (copy_to_user(arg, qsa, sizeof(*qsa)))
		ret = -EFAULT;

	kfree(qsa);
	return ret;
}