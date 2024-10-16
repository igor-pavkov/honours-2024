static u64 blkg_prfill_rwstat_field(struct seq_file *sf,
				    struct blkg_policy_data *pd, int off)
{
	struct blkg_rwstat rwstat = blkg_rwstat_read((void *)pd->blkg + off);

	return __blkg_prfill_rwstat(sf, pd, &rwstat);
}