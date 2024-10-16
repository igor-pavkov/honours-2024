static int ftrace_cmp_recs(const void *a, const void *b)
{
	const struct dyn_ftrace *key = a;
	const struct dyn_ftrace *rec = b;

	if (key->flags < rec->ip)
		return -1;
	if (key->ip >= rec->ip + MCOUNT_INSN_SIZE)
		return 1;
	return 0;
}