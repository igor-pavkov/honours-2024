static inline bool FNAME(is_last_gpte)(struct kvm_mmu *mmu,
				       unsigned int level, unsigned int gpte)
{
	/*
	 * For EPT and PAE paging (both variants), bit 7 is either reserved at
	 * all level or indicates a huge page (ignoring CR3/EPTP).  In either
	 * case, bit 7 being set terminates the walk.
	 */
#if PTTYPE == 32
	/*
	 * 32-bit paging requires special handling because bit 7 is ignored if
	 * CR4.PSE=0, not reserved.  Clear bit 7 in the gpte if the level is
	 * greater than the last level for which bit 7 is the PAGE_SIZE bit.
	 *
	 * The RHS has bit 7 set iff level < (2 + PSE).  If it is clear, bit 7
	 * is not reserved and does not indicate a large page at this level,
	 * so clear PT_PAGE_SIZE_MASK in gpte if that is the case.
	 */
	gpte &= level - (PT32_ROOT_LEVEL + mmu->mmu_role.ext.cr4_pse);
#endif
	/*
	 * PG_LEVEL_4K always terminates.  The RHS has bit 7 set
	 * iff level <= PG_LEVEL_4K, which for our purpose means
	 * level == PG_LEVEL_4K; set PT_PAGE_SIZE_MASK in gpte then.
	 */
	gpte |= level - PG_LEVEL_4K - 1;

	return gpte & PT_PAGE_SIZE_MASK;
}