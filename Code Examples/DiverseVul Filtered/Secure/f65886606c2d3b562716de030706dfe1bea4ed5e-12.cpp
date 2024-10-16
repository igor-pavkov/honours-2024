int kvm_mmu_memory_cache_nr_free_objects(struct kvm_mmu_memory_cache *mc)
{
	return mc->nobjs;
}