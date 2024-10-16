pte_t *huge_pte_alloc(struct mm_struct *mm,
			unsigned long addr, unsigned long sz)
{
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pte_t *pte = NULL;

	pgd = pgd_offset(mm, addr);
	p4d = p4d_offset(pgd, addr);
	pud = pud_alloc(mm, p4d, addr);
	if (pud) {
		if (sz == PUD_SIZE) {
			pte = (pte_t *)pud;
		} else {
			BUG_ON(sz != PMD_SIZE);
			if (want_pmd_share() && pud_none(*pud))
				pte = huge_pmd_share(mm, addr, pud);
			else
				pte = (pte_t *)pmd_alloc(mm, pud, addr);
		}
	}
	BUG_ON(pte && pte_present(*pte) && !pte_huge(*pte));

	return pte;
}