static int is_hugetlb_entry_hwpoisoned(pte_t pte)
{
	swp_entry_t swp;

	if (huge_pte_none(pte) || pte_present(pte))
		return 0;
	swp = pte_to_swp_entry(pte);
	if (non_swap_entry(swp) && is_hwpoison_entry(swp))
		return 1;
	else
		return 0;
}