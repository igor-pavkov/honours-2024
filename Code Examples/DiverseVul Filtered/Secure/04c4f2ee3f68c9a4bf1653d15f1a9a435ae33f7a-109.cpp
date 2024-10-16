static void grow_ple_window(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned int old = vmx->ple_window;

	vmx->ple_window = __grow_ple_window(old, ple_window,
					    ple_window_grow,
					    ple_window_max);

	if (vmx->ple_window != old) {
		vmx->ple_window_dirty = true;
		trace_kvm_ple_window_update(vcpu->vcpu_id,
					    vmx->ple_window, old);
	}
}