static int vmx_create_vcpu(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx;
	int i, cpu, err;

	BUILD_BUG_ON(offsetof(struct vcpu_vmx, vcpu) != 0);
	vmx = to_vmx(vcpu);

	err = -ENOMEM;

	vmx->vpid = allocate_vpid();

	/*
	 * If PML is turned on, failure on enabling PML just results in failure
	 * of creating the vcpu, therefore we can simplify PML logic (by
	 * avoiding dealing with cases, such as enabling PML partially on vcpus
	 * for the guest), etc.
	 */
	if (enable_pml) {
		vmx->pml_pg = alloc_page(GFP_KERNEL_ACCOUNT | __GFP_ZERO);
		if (!vmx->pml_pg)
			goto free_vpid;
	}

	BUILD_BUG_ON(ARRAY_SIZE(vmx_uret_msrs_list) != MAX_NR_USER_RETURN_MSRS);

	for (i = 0; i < ARRAY_SIZE(vmx_uret_msrs_list); ++i) {
		u32 index = vmx_uret_msrs_list[i];
		u32 data_low, data_high;
		int j = vmx->nr_uret_msrs;

		if (rdmsr_safe(index, &data_low, &data_high) < 0)
			continue;
		if (wrmsr_safe(index, data_low, data_high) < 0)
			continue;

		vmx->guest_uret_msrs[j].slot = i;
		vmx->guest_uret_msrs[j].data = 0;
		switch (index) {
		case MSR_IA32_TSX_CTRL:
			/*
			 * TSX_CTRL_CPUID_CLEAR is handled in the CPUID
			 * interception.  Keep the host value unchanged to avoid
			 * changing CPUID bits under the host kernel's feet.
			 *
			 * hle=0, rtm=0, tsx_ctrl=1 can be found with some
			 * combinations of new kernel and old userspace.  If
			 * those guests run on a tsx=off host, do allow guests
			 * to use TSX_CTRL, but do not change the value on the
			 * host so that TSX remains always disabled.
			 */
			if (boot_cpu_has(X86_FEATURE_RTM))
				vmx->guest_uret_msrs[j].mask = ~(u64)TSX_CTRL_CPUID_CLEAR;
			else
				vmx->guest_uret_msrs[j].mask = 0;
			break;
		default:
			vmx->guest_uret_msrs[j].mask = -1ull;
			break;
		}
		++vmx->nr_uret_msrs;
	}

	err = alloc_loaded_vmcs(&vmx->vmcs01);
	if (err < 0)
		goto free_pml;

	/* The MSR bitmap starts with all ones */
	bitmap_fill(vmx->shadow_msr_intercept.read, MAX_POSSIBLE_PASSTHROUGH_MSRS);
	bitmap_fill(vmx->shadow_msr_intercept.write, MAX_POSSIBLE_PASSTHROUGH_MSRS);

	vmx_disable_intercept_for_msr(vcpu, MSR_IA32_TSC, MSR_TYPE_R);
	vmx_disable_intercept_for_msr(vcpu, MSR_FS_BASE, MSR_TYPE_RW);
	vmx_disable_intercept_for_msr(vcpu, MSR_GS_BASE, MSR_TYPE_RW);
	vmx_disable_intercept_for_msr(vcpu, MSR_KERNEL_GS_BASE, MSR_TYPE_RW);
	vmx_disable_intercept_for_msr(vcpu, MSR_IA32_SYSENTER_CS, MSR_TYPE_RW);
	vmx_disable_intercept_for_msr(vcpu, MSR_IA32_SYSENTER_ESP, MSR_TYPE_RW);
	vmx_disable_intercept_for_msr(vcpu, MSR_IA32_SYSENTER_EIP, MSR_TYPE_RW);
	if (kvm_cstate_in_guest(vcpu->kvm)) {
		vmx_disable_intercept_for_msr(vcpu, MSR_CORE_C1_RES, MSR_TYPE_R);
		vmx_disable_intercept_for_msr(vcpu, MSR_CORE_C3_RESIDENCY, MSR_TYPE_R);
		vmx_disable_intercept_for_msr(vcpu, MSR_CORE_C6_RESIDENCY, MSR_TYPE_R);
		vmx_disable_intercept_for_msr(vcpu, MSR_CORE_C7_RESIDENCY, MSR_TYPE_R);
	}
	vmx->msr_bitmap_mode = 0;

	vmx->loaded_vmcs = &vmx->vmcs01;
	cpu = get_cpu();
	vmx_vcpu_load(vcpu, cpu);
	vcpu->cpu = cpu;
	init_vmcs(vmx);
	vmx_vcpu_put(vcpu);
	put_cpu();
	if (cpu_need_virtualize_apic_accesses(vcpu)) {
		err = alloc_apic_access_page(vcpu->kvm);
		if (err)
			goto free_vmcs;
	}

	if (enable_ept && !enable_unrestricted_guest) {
		err = init_rmode_identity_map(vcpu->kvm);
		if (err)
			goto free_vmcs;
	}

	if (nested)
		memcpy(&vmx->nested.msrs, &vmcs_config.nested, sizeof(vmx->nested.msrs));
	else
		memset(&vmx->nested.msrs, 0, sizeof(vmx->nested.msrs));

	vmx->nested.posted_intr_nv = -1;
	vmx->nested.current_vmptr = -1ull;

	vcpu->arch.microcode_version = 0x100000000ULL;
	vmx->msr_ia32_feature_control_valid_bits = FEAT_CTL_LOCKED;

	/*
	 * Enforce invariant: pi_desc.nv is always either POSTED_INTR_VECTOR
	 * or POSTED_INTR_WAKEUP_VECTOR.
	 */
	vmx->pi_desc.nv = POSTED_INTR_VECTOR;
	vmx->pi_desc.sn = 1;

	vmx->ept_pointer = INVALID_PAGE;

	return 0;

free_vmcs:
	free_loaded_vmcs(vmx->loaded_vmcs);
free_pml:
	vmx_destroy_pml_buffer(vmx);
free_vpid:
	free_vpid(vmx->vpid);
	return err;
}