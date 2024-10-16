static __init int hardware_setup(void)
{
	unsigned long host_bndcfgs;
	struct desc_ptr dt;
	int r, i, ept_lpage_level;

	store_idt(&dt);
	host_idt_base = dt.address;

	for (i = 0; i < ARRAY_SIZE(vmx_uret_msrs_list); ++i)
		kvm_define_user_return_msr(i, vmx_uret_msrs_list[i]);

	if (setup_vmcs_config(&vmcs_config, &vmx_capability) < 0)
		return -EIO;

	if (boot_cpu_has(X86_FEATURE_NX))
		kvm_enable_efer_bits(EFER_NX);

	if (boot_cpu_has(X86_FEATURE_MPX)) {
		rdmsrl(MSR_IA32_BNDCFGS, host_bndcfgs);
		WARN_ONCE(host_bndcfgs, "KVM: BNDCFGS in host will be lost");
	}

	if (!cpu_has_vmx_mpx())
		supported_xcr0 &= ~(XFEATURE_MASK_BNDREGS |
				    XFEATURE_MASK_BNDCSR);

	if (!cpu_has_vmx_vpid() || !cpu_has_vmx_invvpid() ||
	    !(cpu_has_vmx_invvpid_single() || cpu_has_vmx_invvpid_global()))
		enable_vpid = 0;

	if (!cpu_has_vmx_ept() ||
	    !cpu_has_vmx_ept_4levels() ||
	    !cpu_has_vmx_ept_mt_wb() ||
	    !cpu_has_vmx_invept_global())
		enable_ept = 0;

	if (!cpu_has_vmx_ept_ad_bits() || !enable_ept)
		enable_ept_ad_bits = 0;

	if (!cpu_has_vmx_unrestricted_guest() || !enable_ept)
		enable_unrestricted_guest = 0;

	if (!cpu_has_vmx_flexpriority())
		flexpriority_enabled = 0;

	if (!cpu_has_virtual_nmis())
		enable_vnmi = 0;

	/*
	 * set_apic_access_page_addr() is used to reload apic access
	 * page upon invalidation.  No need to do anything if not
	 * using the APIC_ACCESS_ADDR VMCS field.
	 */
	if (!flexpriority_enabled)
		vmx_x86_ops.set_apic_access_page_addr = NULL;

	if (!cpu_has_vmx_tpr_shadow())
		vmx_x86_ops.update_cr8_intercept = NULL;

#if IS_ENABLED(CONFIG_HYPERV)
	if (ms_hyperv.nested_features & HV_X64_NESTED_GUEST_MAPPING_FLUSH
	    && enable_ept) {
		vmx_x86_ops.tlb_remote_flush = hv_remote_flush_tlb;
		vmx_x86_ops.tlb_remote_flush_with_range =
				hv_remote_flush_tlb_with_range;
	}
#endif

	if (!cpu_has_vmx_ple()) {
		ple_gap = 0;
		ple_window = 0;
		ple_window_grow = 0;
		ple_window_max = 0;
		ple_window_shrink = 0;
	}

	if (!cpu_has_vmx_apicv()) {
		enable_apicv = 0;
		vmx_x86_ops.sync_pir_to_irr = NULL;
	}

	if (cpu_has_vmx_tsc_scaling()) {
		kvm_has_tsc_control = true;
		kvm_max_tsc_scaling_ratio = KVM_VMX_TSC_MULTIPLIER_MAX;
		kvm_tsc_scaling_ratio_frac_bits = 48;
	}

	kvm_has_bus_lock_exit = cpu_has_vmx_bus_lock_detection();

	set_bit(0, vmx_vpid_bitmap); /* 0 is reserved for host */

	if (enable_ept)
		vmx_enable_tdp();

	if (!enable_ept)
		ept_lpage_level = 0;
	else if (cpu_has_vmx_ept_1g_page())
		ept_lpage_level = PG_LEVEL_1G;
	else if (cpu_has_vmx_ept_2m_page())
		ept_lpage_level = PG_LEVEL_2M;
	else
		ept_lpage_level = PG_LEVEL_4K;
	kvm_configure_mmu(enable_ept, vmx_get_max_tdp_level(), ept_lpage_level);

	/*
	 * Only enable PML when hardware supports PML feature, and both EPT
	 * and EPT A/D bit features are enabled -- PML depends on them to work.
	 */
	if (!enable_ept || !enable_ept_ad_bits || !cpu_has_vmx_pml())
		enable_pml = 0;

	if (!enable_pml)
		vmx_x86_ops.cpu_dirty_log_size = 0;

	if (!cpu_has_vmx_preemption_timer())
		enable_preemption_timer = false;

	if (enable_preemption_timer) {
		u64 use_timer_freq = 5000ULL * 1000 * 1000;
		u64 vmx_msr;

		rdmsrl(MSR_IA32_VMX_MISC, vmx_msr);
		cpu_preemption_timer_multi =
			vmx_msr & VMX_MISC_PREEMPTION_TIMER_RATE_MASK;

		if (tsc_khz)
			use_timer_freq = (u64)tsc_khz * 1000;
		use_timer_freq >>= cpu_preemption_timer_multi;

		/*
		 * KVM "disables" the preemption timer by setting it to its max
		 * value.  Don't use the timer if it might cause spurious exits
		 * at a rate faster than 0.1 Hz (of uninterrupted guest time).
		 */
		if (use_timer_freq > 0xffffffffu / 10)
			enable_preemption_timer = false;
	}

	if (!enable_preemption_timer) {
		vmx_x86_ops.set_hv_timer = NULL;
		vmx_x86_ops.cancel_hv_timer = NULL;
		vmx_x86_ops.request_immediate_exit = __kvm_request_immediate_exit;
	}

	kvm_set_posted_intr_wakeup_handler(pi_wakeup_handler);

	kvm_mce_cap_supported |= MCG_LMCE_P;

	if (pt_mode != PT_MODE_SYSTEM && pt_mode != PT_MODE_HOST_GUEST)
		return -EINVAL;
	if (!enable_ept || !cpu_has_vmx_intel_pt())
		pt_mode = PT_MODE_SYSTEM;

	if (nested) {
		nested_vmx_setup_ctls_msrs(&vmcs_config.nested,
					   vmx_capability.ept);

		r = nested_vmx_hardware_setup(kvm_vmx_exit_handlers);
		if (r)
			return r;
	}

	vmx_set_cpu_caps();

	r = alloc_kvm_area();
	if (r)
		nested_vmx_hardware_unsetup();
	return r;
}