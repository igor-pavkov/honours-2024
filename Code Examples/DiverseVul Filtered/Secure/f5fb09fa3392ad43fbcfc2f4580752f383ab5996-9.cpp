static void __exit exit_minix_fs(void)
{
        unregister_filesystem(&minix_fs_type);
	destroy_inodecache();
}