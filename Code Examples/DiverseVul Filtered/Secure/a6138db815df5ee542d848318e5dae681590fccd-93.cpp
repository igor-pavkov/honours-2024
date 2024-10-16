static int __init set_mhash_entries(char *str)
{
	if (!str)
		return 0;
	mhash_entries = simple_strtoul(str, &str, 0);
	return 1;
}