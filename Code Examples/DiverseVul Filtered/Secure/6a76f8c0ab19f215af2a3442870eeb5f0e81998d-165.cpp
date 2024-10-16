static void free_ftrace_hash_rcu(struct ftrace_hash *hash)
{
	if (!hash || hash == EMPTY_HASH)
		return;
	call_rcu_sched(&hash->rcu, __free_ftrace_hash_rcu);
}