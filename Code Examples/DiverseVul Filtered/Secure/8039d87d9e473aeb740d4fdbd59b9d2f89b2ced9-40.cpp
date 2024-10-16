static noinline int btrfs_ioctl_fitrim(struct file *file, void __user *arg)
{
	struct btrfs_fs_info *fs_info = btrfs_sb(file_inode(file)->i_sb);
	struct btrfs_device *device;
	struct request_queue *q;
	struct fstrim_range range;
	u64 minlen = ULLONG_MAX;
	u64 num_devices = 0;
	u64 total_bytes = btrfs_super_total_bytes(fs_info->super_copy);
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	rcu_read_lock();
	list_for_each_entry_rcu(device, &fs_info->fs_devices->devices,
				dev_list) {
		if (!device->bdev)
			continue;
		q = bdev_get_queue(device->bdev);
		if (blk_queue_discard(q)) {
			num_devices++;
			minlen = min((u64)q->limits.discard_granularity,
				     minlen);
		}
	}
	rcu_read_unlock();

	if (!num_devices)
		return -EOPNOTSUPP;
	if (copy_from_user(&range, arg, sizeof(range)))
		return -EFAULT;
	if (range.start > total_bytes ||
	    range.len < fs_info->sb->s_blocksize)
		return -EINVAL;

	range.len = min(range.len, total_bytes - range.start);
	range.minlen = max(range.minlen, minlen);
	ret = btrfs_trim_fs(fs_info->tree_root, &range);
	if (ret < 0)
		return ret;

	if (copy_to_user(arg, &range, sizeof(range)))
		return -EFAULT;

	return 0;
}