static void nfs_readdir_page_put(struct nfs_readdir_descriptor *desc)
{
	put_page(desc->page);
	desc->page = NULL;
}