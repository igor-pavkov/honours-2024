static ssize_t ext4_ui_proc_write(struct file *file, const char __user *buf,
			       size_t cnt, loff_t *ppos)
{
	unsigned long *p = PDE(file->f_path.dentry->d_inode)->data;
	char str[32];

	if (cnt >= sizeof(str))
		return -EINVAL;
	if (copy_from_user(str, buf, cnt))
		return -EFAULT;

	*p = simple_strtoul(str, NULL, 0);
	return cnt;
}