static void nested_flush_cached_shadow_vmcs12(struct kvm_vcpu *vcpu,
					      struct vmcs12 *vmcs12)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!nested_cpu_has_shadow_vmcs(vmcs12) ||
	    vmcs12->vmcs_link_pointer == -1ull)
		return;

	kvm_write_guest(vmx->vcpu.kvm, vmcs12->vmcs_link_pointer,
			get_shadow_vmcs12(vcpu), VMCS12_SIZE);
}