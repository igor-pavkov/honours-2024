static struct vmcs *alloc_shadow_vmcs(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct loaded_vmcs *loaded_vmcs = vmx->loaded_vmcs;

	/*
	 * We should allocate a shadow vmcs for vmcs01 only when L1
	 * executes VMXON and free it when L1 executes VMXOFF.
	 * As it is invalid to execute VMXON twice, we shouldn't reach
	 * here when vmcs01 already have an allocated shadow vmcs.
	 */
	WARN_ON(loaded_vmcs == &vmx->vmcs01 && loaded_vmcs->shadow_vmcs);

	if (!loaded_vmcs->shadow_vmcs) {
		loaded_vmcs->shadow_vmcs = alloc_vmcs(true);
		if (loaded_vmcs->shadow_vmcs)
			vmcs_clear(loaded_vmcs->shadow_vmcs);
	}
	return loaded_vmcs->shadow_vmcs;
}