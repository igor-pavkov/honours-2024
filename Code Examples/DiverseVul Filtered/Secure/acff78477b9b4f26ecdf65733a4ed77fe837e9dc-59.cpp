static void vmx_switch_vmcs(struct kvm_vcpu *vcpu, struct loaded_vmcs *vmcs)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	int cpu;

	if (vmx->loaded_vmcs == vmcs)
		return;

	cpu = get_cpu();
	vmx_vcpu_put(vcpu);
	vmx->loaded_vmcs = vmcs;
	vmx_vcpu_load(vcpu, cpu);
	put_cpu();

	vm_entry_controls_reset_shadow(vmx);
	vm_exit_controls_reset_shadow(vmx);
	vmx_segment_cache_clear(vmx);
}