static void vmx_free_vcpu(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (enable_pml)
		vmx_destroy_pml_buffer(vmx);
	free_vpid(vmx->vpid);
	nested_vmx_free_vcpu(vcpu);
	free_loaded_vmcs(vmx->loaded_vmcs);
}