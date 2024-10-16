static __inline__ struct fib6_node * node_alloc(void)
{
	struct fib6_node *fn;

	fn = kmem_cache_zalloc(fib6_node_kmem, GFP_ATOMIC);

	return fn;
}