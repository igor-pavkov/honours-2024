static u64 blkg_prfill_rwstat_field_recursive(struct seq_file *sf,
					      struct blkg_policy_data *pd,
					      int off)
{
	struct blkg_rwstat rwstat = blkg_rwstat_recursive_sum(pd->blkg,
							      NULL, off);
	return __blkg_prfill_rwstat(sf, pd, &rwstat);
}