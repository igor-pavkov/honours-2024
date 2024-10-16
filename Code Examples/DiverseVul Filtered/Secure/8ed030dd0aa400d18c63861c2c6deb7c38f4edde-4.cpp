static void ccid_kmem_cache_destroy(struct kmem_cache *slab)
{
	if (slab != NULL)
		kmem_cache_destroy(slab);
}