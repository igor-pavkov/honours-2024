static void vmx_enable_smi_window(struct kvm_vcpu *vcpu)
{
	/* RSM will cause a vmexit anyway.  */
}