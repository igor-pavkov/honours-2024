static void set_vma_resv_flags(struct vm_area_struct *vma, unsigned long flags)
{
	VM_BUG_ON_VMA(!is_vm_hugetlb_page(vma), vma);
	VM_BUG_ON_VMA(vma->vm_flags & VM_MAYSHARE, vma);

	set_vma_private_data(vma, get_vma_private_data(vma) | flags);
}