static int compat_table_info(const struct ebt_table_info *info,
			     struct compat_ebt_replace *newinfo)
{
	unsigned int size = info->entries_size;
	const void *entries = info->entries;

	newinfo->entries_size = size;

	xt_compat_init_offsets(AF_INET, info->nentries);
	return EBT_ENTRY_ITERATE(entries, size, compat_calc_entry, info,
							entries, newinfo);
}