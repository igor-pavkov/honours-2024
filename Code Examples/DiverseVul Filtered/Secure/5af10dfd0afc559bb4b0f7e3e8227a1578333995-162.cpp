pgoff_t linear_hugepage_index(struct vm_area_struct *vma,
				     unsigned long address)
{
	return vma_hugecache_offset(hstate_vma(vma), vma, address);
}