static int vmx_get_nested_state(struct kvm_vcpu *vcpu,
				struct kvm_nested_state __user *user_kvm_nested_state,
				u32 user_data_size)
{
	struct vcpu_vmx *vmx;
	struct vmcs12 *vmcs12;
	struct kvm_nested_state kvm_state = {
		.flags = 0,
		.format = 0,
		.size = sizeof(kvm_state),
		.vmx.vmxon_pa = -1ull,
		.vmx.vmcs_pa = -1ull,
	};

	if (!vcpu)
		return kvm_state.size + 2 * VMCS12_SIZE;

	vmx = to_vmx(vcpu);
	vmcs12 = get_vmcs12(vcpu);

	if (nested_vmx_allowed(vcpu) && vmx->nested.enlightened_vmcs_enabled)
		kvm_state.flags |= KVM_STATE_NESTED_EVMCS;

	if (nested_vmx_allowed(vcpu) &&
	    (vmx->nested.vmxon || vmx->nested.smm.vmxon)) {
		kvm_state.vmx.vmxon_pa = vmx->nested.vmxon_ptr;
		kvm_state.vmx.vmcs_pa = vmx->nested.current_vmptr;

		if (vmx_has_valid_vmcs12(vcpu)) {
			kvm_state.size += VMCS12_SIZE;

			if (is_guest_mode(vcpu) &&
			    nested_cpu_has_shadow_vmcs(vmcs12) &&
			    vmcs12->vmcs_link_pointer != -1ull)
				kvm_state.size += VMCS12_SIZE;
		}

		if (vmx->nested.smm.vmxon)
			kvm_state.vmx.smm.flags |= KVM_STATE_NESTED_SMM_VMXON;

		if (vmx->nested.smm.guest_mode)
			kvm_state.vmx.smm.flags |= KVM_STATE_NESTED_SMM_GUEST_MODE;

		if (is_guest_mode(vcpu)) {
			kvm_state.flags |= KVM_STATE_NESTED_GUEST_MODE;

			if (vmx->nested.nested_run_pending)
				kvm_state.flags |= KVM_STATE_NESTED_RUN_PENDING;
		}
	}

	if (user_data_size < kvm_state.size)
		goto out;

	if (copy_to_user(user_kvm_nested_state, &kvm_state, sizeof(kvm_state)))
		return -EFAULT;

	if (!vmx_has_valid_vmcs12(vcpu))
		goto out;

	/*
	 * When running L2, the authoritative vmcs12 state is in the
	 * vmcs02. When running L1, the authoritative vmcs12 state is
	 * in the shadow or enlightened vmcs linked to vmcs01, unless
	 * need_vmcs12_sync is set, in which case, the authoritative
	 * vmcs12 state is in the vmcs12 already.
	 */
	if (is_guest_mode(vcpu)) {
		sync_vmcs12(vcpu, vmcs12);
	} else if (!vmx->nested.need_vmcs12_sync) {
		if (vmx->nested.hv_evmcs)
			copy_enlightened_to_vmcs12(vmx);
		else if (enable_shadow_vmcs)
			copy_shadow_to_vmcs12(vmx);
	}

	/*
	 * Copy over the full allocated size of vmcs12 rather than just the size
	 * of the struct.
	 */
	if (copy_to_user(user_kvm_nested_state->data, vmcs12, VMCS12_SIZE))
		return -EFAULT;

	if (nested_cpu_has_shadow_vmcs(vmcs12) &&
	    vmcs12->vmcs_link_pointer != -1ull) {
		if (copy_to_user(user_kvm_nested_state->data + VMCS12_SIZE,
				 get_shadow_vmcs12(vcpu), VMCS12_SIZE))
			return -EFAULT;
	}

out:
	return kvm_state.size;
}