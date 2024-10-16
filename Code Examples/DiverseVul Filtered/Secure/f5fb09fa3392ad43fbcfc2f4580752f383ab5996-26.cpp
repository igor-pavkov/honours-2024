static void destroy_inodecache(void)
{
	if (kmem_cache_destroy(minix_inode_cachep))
		printk(KERN_INFO "minix_inode_cache: not all structures were freed\n");
}