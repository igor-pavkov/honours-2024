static void update_eoi_exitmap(struct kvm_vcpu *vcpu)
{
	u64 eoi_exit_bitmap[4];

	memset(eoi_exit_bitmap, 0, 32);

	kvm_ioapic_calculate_eoi_exitmap(vcpu, eoi_exit_bitmap);
	kvm_x86_ops->load_eoi_exitmap(vcpu, eoi_exit_bitmap);
}