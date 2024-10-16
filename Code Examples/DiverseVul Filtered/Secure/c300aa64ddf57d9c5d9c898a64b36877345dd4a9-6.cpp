void kvm_arch_commit_memory_region(struct kvm *kvm,
				struct kvm_userspace_memory_region *mem,
				struct kvm_memory_slot old,
				bool user_alloc)
{

	int nr_mmu_pages = 0, npages = mem->memory_size >> PAGE_SHIFT;

	if ((mem->slot >= KVM_USER_MEM_SLOTS) && old.npages && !npages) {
		int ret;

		ret = vm_munmap(old.userspace_addr,
				old.npages * PAGE_SIZE);
		if (ret < 0)
			printk(KERN_WARNING
			       "kvm_vm_ioctl_set_memory_region: "
			       "failed to munmap memory\n");
	}

	if (!kvm->arch.n_requested_mmu_pages)
		nr_mmu_pages = kvm_mmu_calculate_mmu_pages(kvm);

	if (nr_mmu_pages)
		kvm_mmu_change_mmu_pages(kvm, nr_mmu_pages);
	/*
	 * Write protect all pages for dirty logging.
	 * Existing largepage mappings are destroyed here and new ones will
	 * not be created until the end of the logging.
	 */
	if (npages && (mem->flags & KVM_MEM_LOG_DIRTY_PAGES))
		kvm_mmu_slot_remove_write_access(kvm, mem->slot);
	/*
	 * If memory slot is created, or moved, we need to clear all
	 * mmio sptes.
	 */
	if (npages && old.base_gfn != mem->guest_phys_addr >> PAGE_SHIFT) {
		kvm_mmu_zap_all(kvm);
		kvm_reload_remote_mmus(kvm);
	}
}