static void minix_destroy_inode(struct inode *inode)
{
	kmem_cache_free(minix_inode_cachep, minix_i(inode));
}