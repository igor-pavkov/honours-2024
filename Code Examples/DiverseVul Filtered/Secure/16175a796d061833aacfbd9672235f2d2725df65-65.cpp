static void vmx_set_idt(struct kvm_vcpu *vcpu, struct descriptor_table *dt)
{
	vmcs_write32(GUEST_IDTR_LIMIT, dt->limit);
	vmcs_writel(GUEST_IDTR_BASE, dt->base);
}