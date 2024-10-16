static void enter_pmode(struct kvm_vcpu *vcpu)
{
	unsigned long flags;
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	vmx->emulation_required = 1;
	vcpu->arch.rmode.active = 0;

	vmcs_writel(GUEST_TR_BASE, vcpu->arch.rmode.tr.base);
	vmcs_write32(GUEST_TR_LIMIT, vcpu->arch.rmode.tr.limit);
	vmcs_write32(GUEST_TR_AR_BYTES, vcpu->arch.rmode.tr.ar);

	flags = vmcs_readl(GUEST_RFLAGS);
	flags &= ~(X86_EFLAGS_IOPL | X86_EFLAGS_VM);
	flags |= (vcpu->arch.rmode.save_iopl << IOPL_SHIFT);
	vmcs_writel(GUEST_RFLAGS, flags);

	vmcs_writel(GUEST_CR4, (vmcs_readl(GUEST_CR4) & ~X86_CR4_VME) |
			(vmcs_readl(CR4_READ_SHADOW) & X86_CR4_VME));

	update_exception_bitmap(vcpu);

	if (emulate_invalid_guest_state)
		return;

	fix_pmode_dataseg(VCPU_SREG_ES, &vcpu->arch.rmode.es);
	fix_pmode_dataseg(VCPU_SREG_DS, &vcpu->arch.rmode.ds);
	fix_pmode_dataseg(VCPU_SREG_GS, &vcpu->arch.rmode.gs);
	fix_pmode_dataseg(VCPU_SREG_FS, &vcpu->arch.rmode.fs);

	vmcs_write16(GUEST_SS_SELECTOR, 0);
	vmcs_write32(GUEST_SS_AR_BYTES, 0x93);

	vmcs_write16(GUEST_CS_SELECTOR,
		     vmcs_read16(GUEST_CS_SELECTOR) & ~SELECTOR_RPL_MASK);
	vmcs_write32(GUEST_CS_AR_BYTES, 0x9b);
}