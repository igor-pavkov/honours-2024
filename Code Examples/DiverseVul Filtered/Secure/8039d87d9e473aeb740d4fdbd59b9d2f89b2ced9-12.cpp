static long btrfs_ioctl_fs_info(struct btrfs_root *root, void __user *arg)
{
	struct btrfs_ioctl_fs_info_args *fi_args;
	struct btrfs_device *device;
	struct btrfs_device *next;
	struct btrfs_fs_devices *fs_devices = root->fs_info->fs_devices;
	int ret = 0;

	fi_args = kzalloc(sizeof(*fi_args), GFP_KERNEL);
	if (!fi_args)
		return -ENOMEM;

	mutex_lock(&fs_devices->device_list_mutex);
	fi_args->num_devices = fs_devices->num_devices;
	memcpy(&fi_args->fsid, root->fs_info->fsid, sizeof(fi_args->fsid));

	list_for_each_entry_safe(device, next, &fs_devices->devices, dev_list) {
		if (device->devid > fi_args->max_id)
			fi_args->max_id = device->devid;
	}
	mutex_unlock(&fs_devices->device_list_mutex);

	fi_args->nodesize = root->fs_info->super_copy->nodesize;
	fi_args->sectorsize = root->fs_info->super_copy->sectorsize;
	fi_args->clone_alignment = root->fs_info->super_copy->sectorsize;

	if (copy_to_user(arg, fi_args, sizeof(*fi_args)))
		ret = -EFAULT;

	kfree(fi_args);
	return ret;
}