static long btrfs_ioctl_rm_dev(struct file *file, void __user *arg)
{
	struct btrfs_root *root = BTRFS_I(file_inode(file))->root;
	struct btrfs_ioctl_vol_args *vol_args;
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	ret = mnt_want_write_file(file);
	if (ret)
		return ret;

	vol_args = memdup_user(arg, sizeof(*vol_args));
	if (IS_ERR(vol_args)) {
		ret = PTR_ERR(vol_args);
		goto err_drop;
	}

	vol_args->name[BTRFS_PATH_NAME_MAX] = '\0';

	if (atomic_xchg(&root->fs_info->mutually_exclusive_operation_running,
			1)) {
		ret = BTRFS_ERROR_DEV_EXCL_RUN_IN_PROGRESS;
		goto out;
	}

	mutex_lock(&root->fs_info->volume_mutex);
	ret = btrfs_rm_device(root, vol_args->name);
	mutex_unlock(&root->fs_info->volume_mutex);
	atomic_set(&root->fs_info->mutually_exclusive_operation_running, 0);

	if (!ret)
		btrfs_info(root->fs_info, "disk deleted %s",vol_args->name);

out:
	kfree(vol_args);
err_drop:
	mnt_drop_write_file(file);
	return ret;
}