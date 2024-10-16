int minix_getattr(struct vfsmount *mnt, struct dentry *dentry, struct kstat *stat)
{
	generic_fillattr(dentry->d_inode, stat);
	if (INODE_VERSION(dentry->d_inode) == MINIX_V1)
		stat->blocks = (BLOCK_SIZE / 512) * V1_minix_blocks(stat->size);
	else
		stat->blocks = (BLOCK_SIZE / 512) * V2_minix_blocks(stat->size);
	stat->blksize = BLOCK_SIZE;
	return 0;
}