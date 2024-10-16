static int vmx_set_nested_state(struct kvm_vcpu *vcpu,
				struct kvm_nested_state __user *user_kvm_nested_state,
				struct kvm_nested_state *kvm_state)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct vmcs12 *vmcs12;
	u32 exit_qual;
	int ret;

	if (kvm_state->format != 0)
		return -EINVAL;

	if (kvm_state->flags & KVM_STATE_NESTED_EVMCS)
		nested_enable_evmcs(vcpu, NULL);

	if (!nested_vmx_allowed(vcpu))
		return kvm_state->vmx.vmxon_pa == -1ull ? 0 : -EINVAL;

	if (kvm_state->vmx.vmxon_pa == -1ull) {
		if (kvm_state->vmx.smm.flags)
			return -EINVAL;

		if (kvm_state->vmx.vmcs_pa != -1ull)
			return -EINVAL;

		vmx_leave_nested(vcpu);
		return 0;
	}

	if (!page_address_valid(vcpu, kvm_state->vmx.vmxon_pa))
		return -EINVAL;

	if ((kvm_state->vmx.smm.flags & KVM_STATE_NESTED_SMM_GUEST_MODE) &&
	    (kvm_state->flags & KVM_STATE_NESTED_GUEST_MODE))
		return -EINVAL;

	if (kvm_state->vmx.smm.flags &
	    ~(KVM_STATE_NESTED_SMM_GUEST_MODE | KVM_STATE_NESTED_SMM_VMXON))
		return -EINVAL;

	/*
	 * SMM temporarily disables VMX, so we cannot be in guest mode,
	 * nor can VMLAUNCH/VMRESUME be pending.  Outside SMM, SMM flags
	 * must be zero.
	 */
	if (is_smm(vcpu) ? kvm_state->flags : kvm_state->vmx.smm.flags)
		return -EINVAL;

	if ((kvm_state->vmx.smm.flags & KVM_STATE_NESTED_SMM_GUEST_MODE) &&
	    !(kvm_state->vmx.smm.flags & KVM_STATE_NESTED_SMM_VMXON))
		return -EINVAL;

	vmx_leave_nested(vcpu);
	if (kvm_state->vmx.vmxon_pa == -1ull)
		return 0;

	vmx->nested.vmxon_ptr = kvm_state->vmx.vmxon_pa;
	ret = enter_vmx_operation(vcpu);
	if (ret)
		return ret;

	/* Empty 'VMXON' state is permitted */
	if (kvm_state->size < sizeof(kvm_state) + sizeof(*vmcs12))
		return 0;

	if (kvm_state->vmx.vmcs_pa != -1ull) {
		if (kvm_state->vmx.vmcs_pa == kvm_state->vmx.vmxon_pa ||
		    !page_address_valid(vcpu, kvm_state->vmx.vmcs_pa))
			return -EINVAL;

		set_current_vmptr(vmx, kvm_state->vmx.vmcs_pa);
	} else if (kvm_state->flags & KVM_STATE_NESTED_EVMCS) {
		/*
		 * Sync eVMCS upon entry as we may not have
		 * HV_X64_MSR_VP_ASSIST_PAGE set up yet.
		 */
		vmx->nested.need_vmcs12_sync = true;
	} else {
		return -EINVAL;
	}

	if (kvm_state->vmx.smm.flags & KVM_STATE_NESTED_SMM_VMXON) {
		vmx->nested.smm.vmxon = true;
		vmx->nested.vmxon = false;

		if (kvm_state->vmx.smm.flags & KVM_STATE_NESTED_SMM_GUEST_MODE)
			vmx->nested.smm.guest_mode = true;
	}

	vmcs12 = get_vmcs12(vcpu);
	if (copy_from_user(vmcs12, user_kvm_nested_state->data, sizeof(*vmcs12)))
		return -EFAULT;

	if (vmcs12->hdr.revision_id != VMCS12_REVISION)
		return -EINVAL;

	if (!(kvm_state->flags & KVM_STATE_NESTED_GUEST_MODE))
		return 0;

	vmx->nested.nested_run_pending =
		!!(kvm_state->flags & KVM_STATE_NESTED_RUN_PENDING);

	if (nested_cpu_has_shadow_vmcs(vmcs12) &&
	    vmcs12->vmcs_link_pointer != -1ull) {
		struct vmcs12 *shadow_vmcs12 = get_shadow_vmcs12(vcpu);

		if (kvm_state->size < sizeof(kvm_state) + 2 * sizeof(*vmcs12))
			return -EINVAL;

		if (copy_from_user(shadow_vmcs12,
				   user_kvm_nested_state->data + VMCS12_SIZE,
				   sizeof(*vmcs12)))
			return -EFAULT;

		if (shadow_vmcs12->hdr.revision_id != VMCS12_REVISION ||
		    !shadow_vmcs12->hdr.shadow_vmcs)
			return -EINVAL;
	}

	if (nested_vmx_check_vmentry_prereqs(vcpu, vmcs12) ||
	    nested_vmx_check_vmentry_postreqs(vcpu, vmcs12, &exit_qual))
		return -EINVAL;

	vmx->nested.dirty_vmcs12 = true;
	ret = nested_vmx_enter_non_root_mode(vcpu, false);
	if (ret)
		return -EINVAL;

	return 0;
}