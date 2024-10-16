void hugetlb_fix_reserve_counts(struct inode *inode)
{
	struct hugepage_subpool *spool = subpool_inode(inode);
	long rsv_adjust;

	rsv_adjust = hugepage_subpool_get_pages(spool, 1);
	if (rsv_adjust) {
		struct hstate *h = hstate_inode(inode);

		hugetlb_acct_memory(h, 1);
	}
}