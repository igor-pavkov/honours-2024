static int nfs_readdir_xdr_to_array(struct nfs_readdir_descriptor *desc,
				    __be32 *verf_arg, __be32 *verf_res,
				    struct page **arrays, size_t narrays)
{
	struct page **pages;
	struct page *page = *arrays;
	struct nfs_entry *entry;
	size_t array_size;
	struct inode *inode = file_inode(desc->file);
	size_t dtsize = NFS_SERVER(inode)->dtsize;
	int status = -ENOMEM;

	entry = kzalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry)
		return -ENOMEM;
	entry->cookie = nfs_readdir_page_last_cookie(page);
	entry->fh = nfs_alloc_fhandle();
	entry->fattr = nfs_alloc_fattr_with_label(NFS_SERVER(inode));
	entry->server = NFS_SERVER(inode);
	if (entry->fh == NULL || entry->fattr == NULL)
		goto out;

	array_size = (dtsize + PAGE_SIZE - 1) >> PAGE_SHIFT;
	pages = nfs_readdir_alloc_pages(array_size);
	if (!pages)
		goto out;

	do {
		unsigned int pglen;
		status = nfs_readdir_xdr_filler(desc, verf_arg, entry->cookie,
						pages, dtsize,
						verf_res);
		if (status < 0)
			break;

		pglen = status;
		if (pglen == 0) {
			nfs_readdir_page_set_eof(page);
			break;
		}

		verf_arg = verf_res;

		status = nfs_readdir_page_filler(desc, entry, pages, pglen,
						 arrays, narrays);
	} while (!status && nfs_readdir_page_needs_filling(page));

	nfs_readdir_free_pages(pages, array_size);
out:
	nfs_free_fattr(entry->fattr);
	nfs_free_fhandle(entry->fh);
	kfree(entry);
	return status;
}