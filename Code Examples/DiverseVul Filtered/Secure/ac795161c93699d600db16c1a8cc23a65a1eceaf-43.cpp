static int uncached_readdir(struct nfs_readdir_descriptor *desc)
{
	struct page	**arrays;
	size_t		i, sz = 512;
	__be32		verf[NFS_DIR_VERIFIER_SIZE];
	int		status = -ENOMEM;

	dfprintk(DIRCACHE, "NFS: uncached_readdir() searching for cookie %llu\n",
			(unsigned long long)desc->dir_cookie);

	arrays = kcalloc(sz, sizeof(*arrays), GFP_KERNEL);
	if (!arrays)
		goto out;
	arrays[0] = nfs_readdir_page_array_alloc(desc->dir_cookie, GFP_KERNEL);
	if (!arrays[0])
		goto out;

	desc->page_index = 0;
	desc->last_cookie = desc->dir_cookie;
	desc->duped = 0;

	status = nfs_readdir_xdr_to_array(desc, desc->verf, verf, arrays, sz);

	for (i = 0; !desc->eof && i < sz && arrays[i]; i++) {
		desc->page = arrays[i];
		nfs_do_filldir(desc, verf);
	}
	desc->page = NULL;


	for (i = 0; i < sz && arrays[i]; i++)
		nfs_readdir_page_array_free(arrays[i]);
out:
	kfree(arrays);
	dfprintk(DIRCACHE, "NFS: %s: returns %d\n", __func__, status);
	return status;
}