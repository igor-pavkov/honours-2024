static void handle_exception_nmi_irqoff(struct vcpu_vmx *vmx)
{
	u32 intr_info = vmx_get_intr_info(&vmx->vcpu);

	/* if exit due to PF check for async PF */
	if (is_page_fault(intr_info))
		vmx->vcpu.arch.apf.host_apf_flags = kvm_read_and_reset_apf_flags();
	/* Handle machine checks before interrupts are enabled */
	else if (is_machine_check(intr_info))
		kvm_machine_check();
	/* We need to handle NMIs before interrupts are enabled */
	else if (is_nmi(intr_info))
		handle_interrupt_nmi_irqoff(&vmx->vcpu, intr_info);
}