static unsigned long ftrace_location_range(unsigned long start, unsigned long end)
{
	struct ftrace_page *pg;
	struct dyn_ftrace *rec;
	struct dyn_ftrace key;

	key.ip = start;
	key.flags = end;	/* overload flags, as it is unsigned long */

	for (pg = ftrace_pages_start; pg; pg = pg->next) {
		if (end < pg->records[0].ip ||
		    start >= (pg->records[pg->index - 1].ip + MCOUNT_INSN_SIZE))
			continue;
		rec = bsearch(&key, pg->records, pg->index,
			      sizeof(struct dyn_ftrace),
			      ftrace_cmp_recs);
		if (rec)
			return rec->ip;
	}

	return 0;
}