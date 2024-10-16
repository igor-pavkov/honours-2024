static void vmx_inject_irq(struct kvm_vcpu *vcpu, int irq)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	KVMTRACE_1D(INJ_VIRQ, vcpu, (u32)irq, handler);

	++vcpu->stat.irq_injections;
	if (vcpu->arch.rmode.active) {
		vmx->rmode.irq.pending = true;
		vmx->rmode.irq.vector = irq;
		vmx->rmode.irq.rip = kvm_rip_read(vcpu);
		vmcs_write32(VM_ENTRY_INTR_INFO_FIELD,
			     irq | INTR_TYPE_SOFT_INTR | INTR_INFO_VALID_MASK);
		vmcs_write32(VM_ENTRY_INSTRUCTION_LEN, 1);
		kvm_rip_write(vcpu, vmx->rmode.irq.rip - 1);
		return;
	}
	vmcs_write32(VM_ENTRY_INTR_INFO_FIELD,
			irq | INTR_TYPE_EXT_INTR | INTR_INFO_VALID_MASK);
}