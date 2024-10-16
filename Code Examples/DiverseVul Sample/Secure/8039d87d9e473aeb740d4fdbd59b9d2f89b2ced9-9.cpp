static noinline int btrfs_ioctl_tree_search(struct file *file,
					   void __user *argp)
{
	struct btrfs_ioctl_search_args __user *uargs;
	struct btrfs_ioctl_search_key sk;
	struct inode *inode;
	int ret;
	size_t buf_size;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	uargs = (struct btrfs_ioctl_search_args __user *)argp;

	if (copy_from_user(&sk, &uargs->key, sizeof(sk)))
		return -EFAULT;

	buf_size = sizeof(uargs->buf);

	inode = file_inode(file);
	ret = search_ioctl(inode, &sk, &buf_size, uargs->buf);

	/*
	 * In the origin implementation an overflow is handled by returning a
	 * search header with a len of zero, so reset ret.
	 */
	if (ret == -EOVERFLOW)
		ret = 0;

	if (ret == 0 && copy_to_user(&uargs->key, &sk, sizeof(sk)))
		ret = -EFAULT;
	return ret;
}