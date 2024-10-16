long get_nr_dirty_inodes(void)
{
	/* not actually dirty inodes, but a wild approximation */
	long nr_dirty = get_nr_inodes() - get_nr_inodes_unused();
	return nr_dirty > 0 ? nr_dirty : 0;
}