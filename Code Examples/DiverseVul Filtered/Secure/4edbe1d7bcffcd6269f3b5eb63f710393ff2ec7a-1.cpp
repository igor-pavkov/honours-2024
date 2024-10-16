void dm_interface_exit(void)
{
	misc_deregister(&_dm_misc);
	dm_hash_exit();
}