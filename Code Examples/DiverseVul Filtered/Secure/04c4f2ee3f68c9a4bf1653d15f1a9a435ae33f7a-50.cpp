static void handle_interrupt_nmi_irqoff(struct kvm_vcpu *vcpu, u32 intr_info)
{
	unsigned int vector = intr_info & INTR_INFO_VECTOR_MASK;
	gate_desc *desc = (gate_desc *)host_idt_base + vector;

	kvm_before_interrupt(vcpu);
	vmx_do_interrupt_nmi_irqoff(gate_offset(desc));
	kvm_after_interrupt(vcpu);
}