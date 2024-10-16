static void init_once(void * foo, kmem_cache_t * cachep, unsigned long flags)
{
	struct minix_inode_info *ei = (struct minix_inode_info *) foo;

	if ((flags & (SLAB_CTOR_VERIFY|SLAB_CTOR_CONSTRUCTOR)) ==
	    SLAB_CTOR_CONSTRUCTOR)
		inode_init_once(&ei->vfs_inode);
}