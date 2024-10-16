static struct inode *minix_alloc_inode(struct super_block *sb)
{
	struct minix_inode_info *ei;
	ei = (struct minix_inode_info *)kmem_cache_alloc(minix_inode_cachep, SLAB_KERNEL);
	if (!ei)
		return NULL;
	return &ei->vfs_inode;
}