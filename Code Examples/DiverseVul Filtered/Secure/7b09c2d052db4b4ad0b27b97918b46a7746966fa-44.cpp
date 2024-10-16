static void node_free_rcu(struct rcu_head *head)
{
	struct fib6_node *fn = container_of(head, struct fib6_node, rcu);

	kmem_cache_free(fib6_node_kmem, fn);
}