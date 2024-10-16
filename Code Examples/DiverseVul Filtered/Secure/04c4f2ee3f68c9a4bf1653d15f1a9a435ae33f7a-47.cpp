static int __init vmx_init(void)
{
	int r, cpu;

#if IS_ENABLED(CONFIG_HYPERV)
	/*
	 * Enlightened VMCS usage should be recommended and the host needs
	 * to support eVMCS v1 or above. We can also disable eVMCS support
	 * with module parameter.
	 */
	if (enlightened_vmcs &&
	    ms_hyperv.hints & HV_X64_ENLIGHTENED_VMCS_RECOMMENDED &&
	    (ms_hyperv.nested_features & HV_X64_ENLIGHTENED_VMCS_VERSION) >=
	    KVM_EVMCS_VERSION) {
		int cpu;

		/* Check that we have assist pages on all online CPUs */
		for_each_online_cpu(cpu) {
			if (!hv_get_vp_assist_page(cpu)) {
				enlightened_vmcs = false;
				break;
			}
		}

		if (enlightened_vmcs) {
			pr_info("KVM: vmx: using Hyper-V Enlightened VMCS\n");
			static_branch_enable(&enable_evmcs);
		}

		if (ms_hyperv.nested_features & HV_X64_NESTED_DIRECT_FLUSH)
			vmx_x86_ops.enable_direct_tlbflush
				= hv_enable_direct_tlbflush;

	} else {
		enlightened_vmcs = false;
	}
#endif

	r = kvm_init(&vmx_init_ops, sizeof(struct vcpu_vmx),
		     __alignof__(struct vcpu_vmx), THIS_MODULE);
	if (r)
		return r;

	/*
	 * Must be called after kvm_init() so enable_ept is properly set
	 * up. Hand the parameter mitigation value in which was stored in
	 * the pre module init parser. If no parameter was given, it will
	 * contain 'auto' which will be turned into the default 'cond'
	 * mitigation mode.
	 */
	r = vmx_setup_l1d_flush(vmentry_l1d_flush_param);
	if (r) {
		vmx_exit();
		return r;
	}

	for_each_possible_cpu(cpu) {
		INIT_LIST_HEAD(&per_cpu(loaded_vmcss_on_cpu, cpu));

		pi_init_cpu(cpu);
	}

#ifdef CONFIG_KEXEC_CORE
	rcu_assign_pointer(crash_vmclear_loaded_vmcss,
			   crash_vmclear_local_loaded_vmcss);
#endif
	vmx_check_vmcs12_offsets();

	/*
	 * Shadow paging doesn't have a (further) performance penalty
	 * from GUEST_MAXPHYADDR < HOST_MAXPHYADDR so enable it
	 * by default
	 */
	if (!enable_ept)
		allow_smaller_maxphyaddr = true;

	return 0;
}