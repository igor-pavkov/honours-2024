static void kvm_destroy_vm(struct kvm *kvm)
{
	int i;
	struct mm_struct *mm = kvm->mm;

	kvm_arch_sync_events(kvm);
	raw_spin_lock(&kvm_lock);
	list_del(&kvm->vm_list);
	raw_spin_unlock(&kvm_lock);
	kvm_free_irq_routing(kvm);
	for (i = 0; i < KVM_NR_BUSES; i++)
		kvm_io_bus_destroy(kvm->buses[i]);
	kvm_coalesced_mmio_free(kvm);
#if defined(CONFIG_MMU_NOTIFIER) && defined(KVM_ARCH_WANT_MMU_NOTIFIER)
	mmu_notifier_unregister(&kvm->mmu_notifier, kvm->mm);
#else
	kvm_arch_flush_shadow_all(kvm);
#endif
	kvm_arch_destroy_vm(kvm);
	kvm_free_physmem(kvm);
	cleanup_srcu_struct(&kvm->srcu);
	kvm_arch_free_vm(kvm);
	hardware_disable_all();
	mmdrop(mm);
}