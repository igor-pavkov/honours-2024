static void set_vma_resv_map(struct vm_area_struct *vma, struct resv_map *map)
{
	VM_BUG_ON_VMA(!is_vm_hugetlb_page(vma), vma);
	VM_BUG_ON_VMA(vma->vm_flags & VM_MAYSHARE, vma);

	set_vma_private_data(vma, (get_vma_private_data(vma) &
				HPAGE_RESV_MASK) | (unsigned long)map);
}