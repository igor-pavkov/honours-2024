static int vmx_get_cpl(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!is_protmode(vcpu))
		return 0;

	if (!is_long_mode(vcpu)
	    && (kvm_get_rflags(vcpu) & X86_EFLAGS_VM)) /* if virtual 8086 */
		return 3;

	if (!test_bit(VCPU_EXREG_CPL, (ulong *)&vcpu->arch.regs_avail)) {
		__set_bit(VCPU_EXREG_CPL, (ulong *)&vcpu->arch.regs_avail);
		vmx->cpl = vmx_read_guest_seg_selector(vmx, VCPU_SREG_CS) & 3;
	}

	return vmx->cpl;
}