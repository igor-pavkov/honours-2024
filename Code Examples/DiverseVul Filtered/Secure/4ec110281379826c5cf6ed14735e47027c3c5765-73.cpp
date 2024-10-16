static void __exit exit_ext4_fs(void)
{
	unregister_filesystem(&ext4_fs_type);
#ifdef CONFIG_EXT4DEV_COMPAT
	unregister_filesystem(&ext4dev_fs_type);
#endif
	destroy_inodecache();
	exit_ext4_xattr();
	exit_ext4_mballoc();
	remove_proc_entry("fs/ext4", NULL);
}