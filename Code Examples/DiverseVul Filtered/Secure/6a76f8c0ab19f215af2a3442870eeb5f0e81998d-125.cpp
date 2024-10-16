static void ftrace_add_profile(struct ftrace_profile_stat *stat,
			       struct ftrace_profile *rec)
{
	unsigned long key;

	key = hash_long(rec->ip, ftrace_profile_bits);
	hlist_add_head_rcu(&rec->node, &stat->hash[key]);
}