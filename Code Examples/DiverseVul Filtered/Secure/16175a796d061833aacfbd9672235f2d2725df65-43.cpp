static void ept_update_paging_mode_cr4(unsigned long *hw_cr4,
					struct kvm_vcpu *vcpu)
{
	if (!is_paging(vcpu)) {
		*hw_cr4 &= ~X86_CR4_PAE;
		*hw_cr4 |= X86_CR4_PSE;
	} else if (!(vcpu->arch.cr4 & X86_CR4_PAE))
		*hw_cr4 &= ~X86_CR4_PAE;
}