static void vmx_exit(void)
{
#ifdef CONFIG_KEXEC_CORE
	RCU_INIT_POINTER(crash_vmclear_loaded_vmcss, NULL);
	synchronize_rcu();
#endif

	kvm_exit();

#if IS_ENABLED(CONFIG_HYPERV)
	if (static_branch_unlikely(&enable_evmcs)) {
		int cpu;
		struct hv_vp_assist_page *vp_ap;
		/*
		 * Reset everything to support using non-enlightened VMCS
		 * access later (e.g. when we reload the module with
		 * enlightened_vmcs=0)
		 */
		for_each_online_cpu(cpu) {
			vp_ap =	hv_get_vp_assist_page(cpu);

			if (!vp_ap)
				continue;

			vp_ap->nested_control.features.directhypercall = 0;
			vp_ap->current_nested_vmcs = 0;
			vp_ap->enlighten_vmentry = 0;
		}

		static_branch_disable(&enable_evmcs);
	}
#endif
	vmx_cleanup_l1d_flush();
}