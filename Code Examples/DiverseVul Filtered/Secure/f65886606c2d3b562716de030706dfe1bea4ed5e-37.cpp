int kvm_mmu_topup_memory_cache(struct kvm_mmu_memory_cache *mc, int min)
{
	void *obj;

	if (mc->nobjs >= min)
		return 0;
	while (mc->nobjs < ARRAY_SIZE(mc->objects)) {
		obj = mmu_memory_cache_alloc_obj(mc, GFP_KERNEL_ACCOUNT);
		if (!obj)
			return mc->nobjs >= min ? 0 : -ENOMEM;
		mc->objects[mc->nobjs++] = obj;
	}
	return 0;
}