static inline gfp_t htlb_alloc_mask(struct hstate *h)
{
	if (hugepages_treat_as_movable || hugepage_migration_supported(h))
		return GFP_HIGHUSER_MOVABLE;
	else
		return GFP_HIGHUSER;
}