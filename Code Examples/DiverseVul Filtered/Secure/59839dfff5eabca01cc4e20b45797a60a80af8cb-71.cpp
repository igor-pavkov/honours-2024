int kvm_vm_ioctl_get_dirty_log(struct kvm *kvm,
				      struct kvm_dirty_log *log)
{
	int r;
	int n;
	struct kvm_memory_slot *memslot;
	int is_dirty = 0;

	down_write(&kvm->slots_lock);

	r = kvm_get_dirty_log(kvm, log, &is_dirty);
	if (r)
		goto out;

	/* If nothing is dirty, don't bother messing with page tables. */
	if (is_dirty) {
		kvm_mmu_slot_remove_write_access(kvm, log->slot);
		kvm_flush_remote_tlbs(kvm);
		memslot = &kvm->memslots[log->slot];
		n = ALIGN(memslot->npages, BITS_PER_LONG) / 8;
		memset(memslot->dirty_bitmap, 0, n);
	}
	r = 0;
out:
	up_write(&kvm->slots_lock);
	return r;
}