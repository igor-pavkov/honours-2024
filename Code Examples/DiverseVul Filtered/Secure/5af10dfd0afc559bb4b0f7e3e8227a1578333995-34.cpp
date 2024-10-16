void unmap_hugepage_range(struct vm_area_struct *vma, unsigned long start,
			  unsigned long end, struct page *ref_page)
{
	struct mm_struct *mm;
	struct mmu_gather tlb;

	mm = vma->vm_mm;

	tlb_gather_mmu(&tlb, mm, start, end);
	__unmap_hugepage_range(&tlb, vma, start, end, ref_page);
	tlb_finish_mmu(&tlb, start, end);
}