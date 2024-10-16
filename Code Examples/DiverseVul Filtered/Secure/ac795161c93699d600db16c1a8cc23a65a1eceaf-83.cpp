static int find_and_lock_cache_page(struct nfs_readdir_descriptor *desc)
{
	struct inode *inode = file_inode(desc->file);
	struct nfs_inode *nfsi = NFS_I(inode);
	__be32 verf[NFS_DIR_VERIFIER_SIZE];
	int res;

	desc->page = nfs_readdir_page_get_cached(desc);
	if (!desc->page)
		return -ENOMEM;
	if (nfs_readdir_page_needs_filling(desc->page)) {
		res = nfs_readdir_xdr_to_array(desc, nfsi->cookieverf, verf,
					       &desc->page, 1);
		if (res < 0) {
			nfs_readdir_page_unlock_and_put_cached(desc);
			if (res == -EBADCOOKIE || res == -ENOTSYNC) {
				invalidate_inode_pages2(desc->file->f_mapping);
				desc->page_index = 0;
				return -EAGAIN;
			}
			return res;
		}
		/*
		 * Set the cookie verifier if the page cache was empty
		 */
		if (desc->page_index == 0)
			memcpy(nfsi->cookieverf, verf,
			       sizeof(nfsi->cookieverf));
	}
	res = nfs_readdir_search_array(desc);
	if (res == 0)
		return 0;
	nfs_readdir_page_unlock_and_put_cached(desc);
	return res;
}