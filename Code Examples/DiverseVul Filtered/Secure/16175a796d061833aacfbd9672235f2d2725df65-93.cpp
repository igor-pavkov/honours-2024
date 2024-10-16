static int handle_task_switch(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned long exit_qualification;
	u16 tss_selector;
	int reason;

	exit_qualification = vmcs_readl(EXIT_QUALIFICATION);

	reason = (u32)exit_qualification >> 30;
	if (reason == TASK_SWITCH_GATE && vmx->vcpu.arch.nmi_injected &&
	    (vmx->idt_vectoring_info & VECTORING_INFO_VALID_MASK) &&
	    (vmx->idt_vectoring_info & VECTORING_INFO_TYPE_MASK)
	    == INTR_TYPE_NMI_INTR) {
		vcpu->arch.nmi_injected = false;
		if (cpu_has_virtual_nmis())
			vmcs_set_bits(GUEST_INTERRUPTIBILITY_INFO,
				      GUEST_INTR_STATE_NMI);
	}
	tss_selector = exit_qualification;

	if (!kvm_task_switch(vcpu, tss_selector, reason))
		return 0;

	/* clear all local breakpoint enable flags */
	vmcs_writel(GUEST_DR7, vmcs_readl(GUEST_DR7) & ~55);

	/*
	 * TODO: What about debug traps on tss switch?
	 *       Are we supposed to inject them and update dr6?
	 */

	return 1;
}