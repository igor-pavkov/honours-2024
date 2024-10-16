static void __free_ftrace_hash_rcu(struct rcu_head *rcu)
{
	struct ftrace_hash *hash;

	hash = container_of(rcu, struct ftrace_hash, rcu);
	free_ftrace_hash(hash);
}