static void vmx_free_vmcs(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (vmx->vmcs) {
		vcpu_clear(vmx);
		free_vmcs(vmx->vmcs);
		vmx->vmcs = NULL;
	}
}