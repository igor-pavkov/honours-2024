static long vma_needs_reservation(struct hstate *h,
			struct vm_area_struct *vma, unsigned long addr)
{
	return __vma_reservation_common(h, vma, addr, VMA_NEEDS_RESV);
}