static void handle_external_interrupt_irqoff(struct kvm_vcpu *vcpu)
{
	u32 intr_info = vmx_get_intr_info(vcpu);

	if (WARN_ONCE(!is_external_intr(intr_info),
	    "KVM: unexpected VM-Exit interrupt info: 0x%x", intr_info))
		return;

	handle_interrupt_nmi_irqoff(vcpu, intr_info);
}