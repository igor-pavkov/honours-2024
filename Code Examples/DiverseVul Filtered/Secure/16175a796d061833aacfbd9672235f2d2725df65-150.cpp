static int vmx_get_cpl(struct kvm_vcpu *vcpu)
{
	struct kvm_segment kvm_seg;

	if (!(vcpu->arch.cr0 & X86_CR0_PE)) /* if real mode */
		return 0;

	if (vmx_get_rflags(vcpu) & X86_EFLAGS_VM) /* if virtual 8086 */
		return 3;

	vmx_get_segment(vcpu, &kvm_seg, VCPU_SREG_CS);
	return kvm_seg.selector & 3;
}