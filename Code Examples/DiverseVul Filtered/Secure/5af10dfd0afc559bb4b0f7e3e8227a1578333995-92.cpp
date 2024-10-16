static inline struct hugepage_subpool *subpool_inode(struct inode *inode)
{
	return HUGETLBFS_SB(inode->i_sb)->spool;
}