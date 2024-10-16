static bool nfs_readdir_dont_search_cache(struct nfs_readdir_descriptor *desc)
{
	struct address_space *mapping = desc->file->f_mapping;
	struct inode *dir = file_inode(desc->file);
	unsigned int dtsize = NFS_SERVER(dir)->dtsize;
	loff_t size = i_size_read(dir);

	/*
	 * Default to uncached readdir if the page cache is empty, and
	 * we're looking for a non-zero cookie in a large directory.
	 */
	return desc->dir_cookie != 0 && mapping->nrpages == 0 && size > dtsize;
}