static void vmx_decache_cr4_guest_bits(struct kvm_vcpu *vcpu)
{
	vcpu->arch.cr4 &= KVM_GUEST_CR4_MASK;
	vcpu->arch.cr4 |= vmcs_readl(GUEST_CR4) & ~KVM_GUEST_CR4_MASK;
}