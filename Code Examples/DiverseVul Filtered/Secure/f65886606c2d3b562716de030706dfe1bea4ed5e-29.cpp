static void kvm_mmu_notifier_invalidate_range(struct mmu_notifier *mn,
					      struct mm_struct *mm,
					      unsigned long start, unsigned long end)
{
	struct kvm *kvm = mmu_notifier_to_kvm(mn);
	int idx;

	idx = srcu_read_lock(&kvm->srcu);
	kvm_arch_mmu_notifier_invalidate_range(kvm, start, end);
	srcu_read_unlock(&kvm->srcu, idx);
}