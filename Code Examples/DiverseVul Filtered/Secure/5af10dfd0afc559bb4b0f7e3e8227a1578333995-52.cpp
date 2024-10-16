unsigned long hugetlb_change_protection(struct vm_area_struct *vma,
		unsigned long address, unsigned long end, pgprot_t newprot)
{
	struct mm_struct *mm = vma->vm_mm;
	unsigned long start = address;
	pte_t *ptep;
	pte_t pte;
	struct hstate *h = hstate_vma(vma);
	unsigned long pages = 0;

	BUG_ON(address >= end);
	flush_cache_range(vma, address, end);

	mmu_notifier_invalidate_range_start(mm, start, end);
	i_mmap_lock_write(vma->vm_file->f_mapping);
	for (; address < end; address += huge_page_size(h)) {
		spinlock_t *ptl;
		ptep = huge_pte_offset(mm, address, huge_page_size(h));
		if (!ptep)
			continue;
		ptl = huge_pte_lock(h, mm, ptep);
		if (huge_pmd_unshare(mm, &address, ptep)) {
			pages++;
			spin_unlock(ptl);
			continue;
		}
		pte = huge_ptep_get(ptep);
		if (unlikely(is_hugetlb_entry_hwpoisoned(pte))) {
			spin_unlock(ptl);
			continue;
		}
		if (unlikely(is_hugetlb_entry_migration(pte))) {
			swp_entry_t entry = pte_to_swp_entry(pte);

			if (is_write_migration_entry(entry)) {
				pte_t newpte;

				make_migration_entry_read(&entry);
				newpte = swp_entry_to_pte(entry);
				set_huge_swap_pte_at(mm, address, ptep,
						     newpte, huge_page_size(h));
				pages++;
			}
			spin_unlock(ptl);
			continue;
		}
		if (!huge_pte_none(pte)) {
			pte = huge_ptep_get_and_clear(mm, address, ptep);
			pte = pte_mkhuge(huge_pte_modify(pte, newprot));
			pte = arch_make_huge_pte(pte, vma, NULL, 0);
			set_huge_pte_at(mm, address, ptep, pte);
			pages++;
		}
		spin_unlock(ptl);
	}
	/*
	 * Must flush TLB before releasing i_mmap_rwsem: x86's huge_pmd_unshare
	 * may have cleared our pud entry and done put_page on the page table:
	 * once we release i_mmap_rwsem, another task can do the final put_page
	 * and that page table be reused and filled with junk.
	 */
	flush_hugetlb_tlb_range(vma, start, end);
	mmu_notifier_invalidate_range(mm, start, end);
	i_mmap_unlock_write(vma->vm_file->f_mapping);
	mmu_notifier_invalidate_range_end(mm, start, end);

	return pages << h->order;
}