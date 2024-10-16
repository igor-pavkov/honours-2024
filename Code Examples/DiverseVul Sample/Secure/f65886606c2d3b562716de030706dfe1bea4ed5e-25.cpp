void kvm_mmu_free_memory_cache(struct kvm_mmu_memory_cache *mc)
{
	while (mc->nobjs) {
		if (mc->kmem_cache)
			kmem_cache_free(mc->kmem_cache, mc->objects[--mc->nobjs]);
		else
			free_page((unsigned long)mc->objects[--mc->nobjs]);
	}
}