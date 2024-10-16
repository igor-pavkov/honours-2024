static fastpath_t handle_fastpath_preemption_timer(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!vmx->req_immediate_exit &&
	    !unlikely(vmx->loaded_vmcs->hv_timer_soft_disabled)) {
		kvm_lapic_expired_hv_timer(vcpu);
		return EXIT_FASTPATH_REENTER_GUEST;
	}

	return EXIT_FASTPATH_NONE;
}