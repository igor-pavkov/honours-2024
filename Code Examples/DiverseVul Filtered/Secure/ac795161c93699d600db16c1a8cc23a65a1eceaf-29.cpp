static const char *nfs_readdir_copy_name(const char *name, unsigned int len)
{
	const char *ret = kmemdup_nul(name, len, GFP_KERNEL);

	/*
	 * Avoid a kmemleak false positive. The pointer to the name is stored
	 * in a page cache page which kmemleak does not scan.
	 */
	if (ret != NULL)
		kmemleak_not_leak(ret);
	return ret;
}