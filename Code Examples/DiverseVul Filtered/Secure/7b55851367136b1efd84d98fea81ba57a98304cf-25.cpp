struct vm_area_struct *vm_area_dup(struct vm_area_struct *orig)
{
	struct vm_area_struct *new = kmem_cache_alloc(vm_area_cachep, GFP_KERNEL);

	if (new) {
		*new = *orig;
		INIT_LIST_HEAD(&new->anon_vma_chain);
	}
	return new;
}