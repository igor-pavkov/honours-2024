bool is_hugetlb_entry_migration(pte_t pte)
{
	swp_entry_t swp;

	if (huge_pte_none(pte) || pte_present(pte))
		return false;
	swp = pte_to_swp_entry(pte);
	if (non_swap_entry(swp) && is_migration_entry(swp))
		return true;
	else
		return false;
}