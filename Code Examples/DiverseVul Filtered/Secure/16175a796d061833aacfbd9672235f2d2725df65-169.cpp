static void vmx_set_segment(struct kvm_vcpu *vcpu,
			    struct kvm_segment *var, int seg)
{
	struct kvm_vmx_segment_field *sf = &kvm_vmx_segment_fields[seg];
	u32 ar;

	if (vcpu->arch.rmode.active && seg == VCPU_SREG_TR) {
		vcpu->arch.rmode.tr.selector = var->selector;
		vcpu->arch.rmode.tr.base = var->base;
		vcpu->arch.rmode.tr.limit = var->limit;
		vcpu->arch.rmode.tr.ar = vmx_segment_access_rights(var);
		return;
	}
	vmcs_writel(sf->base, var->base);
	vmcs_write32(sf->limit, var->limit);
	vmcs_write16(sf->selector, var->selector);
	if (vcpu->arch.rmode.active && var->s) {
		/*
		 * Hack real-mode segments into vm86 compatibility.
		 */
		if (var->base == 0xffff0000 && var->selector == 0xf000)
			vmcs_writel(sf->base, 0xf0000);
		ar = 0xf3;
	} else
		ar = vmx_segment_access_rights(var);
	vmcs_write32(sf->ar_bytes, ar);
}