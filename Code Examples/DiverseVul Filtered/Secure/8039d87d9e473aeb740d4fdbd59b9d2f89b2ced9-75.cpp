static int btrfs_ioctl_getflags(struct file *file, void __user *arg)
{
	struct btrfs_inode *ip = BTRFS_I(file_inode(file));
	unsigned int flags = btrfs_flags_to_ioctl(ip->flags);

	if (copy_to_user(arg, &flags, sizeof(flags)))
		return -EFAULT;
	return 0;
}