static int __init init_ext4_fs(void)
{
	int err;

	ext4_proc_root = proc_mkdir("fs/ext4", NULL);
	err = init_ext4_mballoc();
	if (err)
		return err;

	err = init_ext4_xattr();
	if (err)
		goto out2;
	err = init_inodecache();
	if (err)
		goto out1;
	err = register_filesystem(&ext4_fs_type);
	if (err)
		goto out;
#ifdef CONFIG_EXT4DEV_COMPAT
	err = register_filesystem(&ext4dev_fs_type);
	if (err) {
		unregister_filesystem(&ext4_fs_type);
		goto out;
	}
#endif
	return 0;
out:
	destroy_inodecache();
out1:
	exit_ext4_xattr();
out2:
	exit_ext4_mballoc();
	return err;
}