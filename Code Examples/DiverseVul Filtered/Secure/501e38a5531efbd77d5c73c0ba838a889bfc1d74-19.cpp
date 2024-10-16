static void __exit cleanup (void)
{
	pr_debug ("unregister %s\n", shortname);
	unregister_filesystem (&gadgetfs_type);
}