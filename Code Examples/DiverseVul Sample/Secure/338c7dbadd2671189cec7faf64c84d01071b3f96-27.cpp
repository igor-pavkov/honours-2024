static struct kvm_memslots *install_new_memslots(struct kvm *kvm,
		struct kvm_memslots *slots, struct kvm_memory_slot *new)
{
	struct kvm_memslots *old_memslots = kvm->memslots;

	update_memslots(slots, new, kvm->memslots->generation);
	rcu_assign_pointer(kvm->memslots, slots);
	synchronize_srcu_expedited(&kvm->srcu);

	kvm_arch_memslots_updated(kvm);

	return old_memslots;
}