void kvm_arch_vcpu_uninit(struct kvm_vcpu *vcpu)
{
	kvm_free_lapic(vcpu);
	down_read(&vcpu->kvm->slots_lock);
	kvm_mmu_destroy(vcpu);
	up_read(&vcpu->kvm->slots_lock);
	free_page((unsigned long)vcpu->arch.pio_data);
}