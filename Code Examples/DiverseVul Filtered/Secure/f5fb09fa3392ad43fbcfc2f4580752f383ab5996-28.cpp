static int init_inodecache(void)
{
	minix_inode_cachep = kmem_cache_create("minix_inode_cache",
					     sizeof(struct minix_inode_info),
					     0, (SLAB_RECLAIM_ACCOUNT|
						SLAB_MEM_SPREAD),
					     init_once, NULL);
	if (minix_inode_cachep == NULL)
		return -ENOMEM;
	return 0;
}