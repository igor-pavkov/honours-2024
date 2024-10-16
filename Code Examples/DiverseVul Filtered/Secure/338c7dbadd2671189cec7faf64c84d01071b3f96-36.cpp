void kvm_free_physmem(struct kvm *kvm)
{
	struct kvm_memslots *slots = kvm->memslots;
	struct kvm_memory_slot *memslot;

	kvm_for_each_memslot(memslot, slots)
		kvm_free_physmem_slot(kvm, memslot, NULL);

	kfree(kvm->memslots);
}