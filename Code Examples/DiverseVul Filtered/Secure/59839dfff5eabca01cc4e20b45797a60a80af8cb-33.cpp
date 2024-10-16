static void vapic_exit(struct kvm_vcpu *vcpu)
{
	struct kvm_lapic *apic = vcpu->arch.apic;

	if (!apic || !apic->vapic_addr)
		return;

	down_read(&vcpu->kvm->slots_lock);
	kvm_release_page_dirty(apic->vapic_page);
	mark_page_dirty(vcpu->kvm, apic->vapic_addr >> PAGE_SHIFT);
	up_read(&vcpu->kvm->slots_lock);
}