static __inline__ void node_free(struct fib6_node * fn)
{
	kmem_cache_free(fib6_node_kmem, fn);
}