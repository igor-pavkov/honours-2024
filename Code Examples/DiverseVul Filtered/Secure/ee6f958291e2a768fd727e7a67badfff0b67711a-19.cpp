asmlinkage long sys_mount(char __user * dev_name, char __user * dir_name,
			  char __user * type, unsigned long flags,
			  void __user * data)
{
	int retval;
	unsigned long data_page;
	unsigned long type_page;
	unsigned long dev_page;
	char *dir_page;

	retval = copy_mount_options(type, &type_page);
	if (retval < 0)
		return retval;

	dir_page = getname(dir_name);
	retval = PTR_ERR(dir_page);
	if (IS_ERR(dir_page))
		goto out1;

	retval = copy_mount_options(dev_name, &dev_page);
	if (retval < 0)
		goto out2;

	retval = copy_mount_options(data, &data_page);
	if (retval < 0)
		goto out3;

	lock_kernel();
	retval = do_mount((char *)dev_page, dir_page, (char *)type_page,
			  flags, (void *)data_page);
	unlock_kernel();
	free_page(data_page);

out3:
	free_page(dev_page);
out2:
	putname(dir_page);
out1:
	free_page(type_page);
	return retval;
}