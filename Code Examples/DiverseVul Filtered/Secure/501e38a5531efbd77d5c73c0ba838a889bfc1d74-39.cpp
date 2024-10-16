static int __init init (void)
{
	int status;

	status = register_filesystem (&gadgetfs_type);
	if (status == 0)
		pr_info ("%s: %s, version " DRIVER_VERSION "\n",
			shortname, driver_desc);
	return status;
}