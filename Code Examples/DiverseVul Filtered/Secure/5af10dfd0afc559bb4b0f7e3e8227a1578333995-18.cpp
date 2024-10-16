pte_t *huge_pmd_share(struct mm_struct *mm, unsigned long addr, pud_t *pud)
{
	struct vm_area_struct *vma = find_vma(mm, addr);
	struct address_space *mapping = vma->vm_file->f_mapping;
	pgoff_t idx = ((addr - vma->vm_start) >> PAGE_SHIFT) +
			vma->vm_pgoff;
	struct vm_area_struct *svma;
	unsigned long saddr;
	pte_t *spte = NULL;
	pte_t *pte;
	spinlock_t *ptl;

	if (!vma_shareable(vma, addr))
		return (pte_t *)pmd_alloc(mm, pud, addr);

	i_mmap_lock_write(mapping);
	vma_interval_tree_foreach(svma, &mapping->i_mmap, idx, idx) {
		if (svma == vma)
			continue;

		saddr = page_table_shareable(svma, vma, addr, idx);
		if (saddr) {
			spte = huge_pte_offset(svma->vm_mm, saddr,
					       vma_mmu_pagesize(svma));
			if (spte) {
				get_page(virt_to_page(spte));
				break;
			}
		}
	}

	if (!spte)
		goto out;

	ptl = huge_pte_lock(hstate_vma(vma), mm, spte);
	if (pud_none(*pud)) {
		pud_populate(mm, pud,
				(pmd_t *)((unsigned long)spte & PAGE_MASK));
		mm_inc_nr_pmds(mm);
	} else {
		put_page(virt_to_page(spte));
	}
	spin_unlock(ptl);
out:
	pte = (pte_t *)pmd_alloc(mm, pud, addr);
	i_mmap_unlock_write(mapping);
	return pte;
}