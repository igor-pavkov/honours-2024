static void fixup_rmode_irq(struct vcpu_vmx *vmx)
{
	vmx->rmode.irq.pending = 0;
	if (kvm_rip_read(&vmx->vcpu) + 1 != vmx->rmode.irq.rip)
		return;
	kvm_rip_write(&vmx->vcpu, vmx->rmode.irq.rip);
	if (vmx->idt_vectoring_info & VECTORING_INFO_VALID_MASK) {
		vmx->idt_vectoring_info &= ~VECTORING_INFO_TYPE_MASK;
		vmx->idt_vectoring_info |= INTR_TYPE_EXT_INTR;
		return;
	}
	vmx->idt_vectoring_info =
		VECTORING_INFO_VALID_MASK
		| INTR_TYPE_EXT_INTR
		| vmx->rmode.irq.vector;
}