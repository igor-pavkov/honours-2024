static int __init fail_futex_debugfs(void)
{
	umode_t mode = S_IFREG | S_IRUSR | S_IWUSR;
	struct dentry *dir;

	dir = fault_create_debugfs_attr("fail_futex", NULL,
					&fail_futex.attr);
	if (IS_ERR(dir))
		return PTR_ERR(dir);

	if (!debugfs_create_bool("ignore-private", mode, dir,
				 &fail_futex.ignore_private)) {
		debugfs_remove_recursive(dir);
		return -ENOMEM;
	}

	return 0;
}