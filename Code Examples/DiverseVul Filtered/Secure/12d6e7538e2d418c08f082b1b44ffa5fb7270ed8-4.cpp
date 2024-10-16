static pfn_t __gfn_to_pfn(struct kvm *kvm, gfn_t gfn, bool atomic, bool *async,
			  bool write_fault, bool *writable)
{
	struct kvm_memory_slot *slot;

	if (async)
		*async = false;

	slot = gfn_to_memslot(kvm, gfn);

	return __gfn_to_pfn_memslot(slot, gfn, atomic, async, write_fault,
				    writable);
}