void set_cr4_guest_host_mask(struct vcpu_vmx *vmx)
{
	struct kvm_vcpu *vcpu = &vmx->vcpu;

	vcpu->arch.cr4_guest_owned_bits = KVM_POSSIBLE_CR4_GUEST_BITS &
					  ~vcpu->arch.cr4_guest_rsvd_bits;
	if (!enable_ept)
		vcpu->arch.cr4_guest_owned_bits &= ~X86_CR4_PGE;
	if (is_guest_mode(&vmx->vcpu))
		vcpu->arch.cr4_guest_owned_bits &=
			~get_vmcs12(vcpu)->cr4_guest_host_mask;
	vmcs_writel(CR4_GUEST_HOST_MASK, ~vcpu->arch.cr4_guest_owned_bits);
}