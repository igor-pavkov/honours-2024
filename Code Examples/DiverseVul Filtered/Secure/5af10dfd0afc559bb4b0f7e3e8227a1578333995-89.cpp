static void hugetlb_vm_op_close(struct vm_area_struct *vma)
{
	struct hstate *h = hstate_vma(vma);
	struct resv_map *resv = vma_resv_map(vma);
	struct hugepage_subpool *spool = subpool_vma(vma);
	unsigned long reserve, start, end;
	long gbl_reserve;

	if (!resv || !is_vma_resv_set(vma, HPAGE_RESV_OWNER))
		return;

	start = vma_hugecache_offset(h, vma, vma->vm_start);
	end = vma_hugecache_offset(h, vma, vma->vm_end);

	reserve = (end - start) - region_count(resv, start, end);

	kref_put(&resv->refs, resv_map_release);

	if (reserve) {
		/*
		 * Decrement reserve counts.  The global reserve count may be
		 * adjusted if the subpool has a minimum size.
		 */
		gbl_reserve = hugepage_subpool_put_pages(spool, reserve);
		hugetlb_acct_memory(h, -gbl_reserve);
	}
}