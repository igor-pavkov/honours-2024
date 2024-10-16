static void vmx_handle_exit_irqoff(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (vmx->exit_reason.basic == EXIT_REASON_EXTERNAL_INTERRUPT)
		handle_external_interrupt_irqoff(vcpu);
	else if (vmx->exit_reason.basic == EXIT_REASON_EXCEPTION_NMI)
		handle_exception_nmi_irqoff(vmx);
}