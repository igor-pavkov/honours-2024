int kvm_arch_set_memory_region(struct kvm *kvm,
				struct kvm_userspace_memory_region *mem,
				struct kvm_memory_slot old,
				int user_alloc)
{
	int npages = mem->memory_size >> PAGE_SHIFT;
	struct kvm_memory_slot *memslot = &kvm->memslots[mem->slot];

	/*To keep backward compatibility with older userspace,
	 *x86 needs to hanlde !user_alloc case.
	 */
	if (!user_alloc) {
		if (npages && !old.rmap) {
			unsigned long userspace_addr;

			down_write(&current->mm->mmap_sem);
			userspace_addr = do_mmap(NULL, 0,
						 npages * PAGE_SIZE,
						 PROT_READ | PROT_WRITE,
						 MAP_PRIVATE | MAP_ANONYMOUS,
						 0);
			up_write(&current->mm->mmap_sem);

			if (IS_ERR((void *)userspace_addr))
				return PTR_ERR((void *)userspace_addr);

			/* set userspace_addr atomically for kvm_hva_to_rmapp */
			spin_lock(&kvm->mmu_lock);
			memslot->userspace_addr = userspace_addr;
			spin_unlock(&kvm->mmu_lock);
		} else {
			if (!old.user_alloc && old.rmap) {
				int ret;

				down_write(&current->mm->mmap_sem);
				ret = do_munmap(current->mm, old.userspace_addr,
						old.npages * PAGE_SIZE);
				up_write(&current->mm->mmap_sem);
				if (ret < 0)
					printk(KERN_WARNING
				       "kvm_vm_ioctl_set_memory_region: "
				       "failed to munmap memory\n");
			}
		}
	}

	if (!kvm->arch.n_requested_mmu_pages) {
		unsigned int nr_mmu_pages = kvm_mmu_calculate_mmu_pages(kvm);
		kvm_mmu_change_mmu_pages(kvm, nr_mmu_pages);
	}

	kvm_mmu_slot_remove_write_access(kvm, mem->slot);
	kvm_flush_remote_tlbs(kvm);

	return 0;
}