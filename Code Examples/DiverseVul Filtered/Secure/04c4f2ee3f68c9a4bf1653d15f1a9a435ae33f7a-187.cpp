void vmx_update_msr_bitmap(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u8 mode = vmx_msr_bitmap_mode(vcpu);
	u8 changed = mode ^ vmx->msr_bitmap_mode;

	if (!changed)
		return;

	if (changed & (MSR_BITMAP_MODE_X2APIC | MSR_BITMAP_MODE_X2APIC_APICV))
		vmx_update_msr_bitmap_x2apic(vcpu, mode);

	vmx->msr_bitmap_mode = mode;
}