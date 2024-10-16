int huge_pmd_unshare(struct mm_struct *mm, unsigned long *addr, pte_t *ptep)
{
	pgd_t *pgd = pgd_offset(mm, *addr);
	p4d_t *p4d = p4d_offset(pgd, *addr);
	pud_t *pud = pud_offset(p4d, *addr);

	BUG_ON(page_count(virt_to_page(ptep)) == 0);
	if (page_count(virt_to_page(ptep)) == 1)
		return 0;

	pud_clear(pud);
	put_page(virt_to_page(ptep));
	mm_dec_nr_pmds(mm);
	*addr = ALIGN(*addr, HPAGE_SIZE * PTRS_PER_PTE) - HPAGE_SIZE;
	return 1;
}