static int __init vmx_check_processor_compat(void)
{
	struct vmcs_config vmcs_conf;
	struct vmx_capability vmx_cap;

	if (!this_cpu_has(X86_FEATURE_MSR_IA32_FEAT_CTL) ||
	    !this_cpu_has(X86_FEATURE_VMX)) {
		pr_err("kvm: VMX is disabled on CPU %d\n", smp_processor_id());
		return -EIO;
	}

	if (setup_vmcs_config(&vmcs_conf, &vmx_cap) < 0)
		return -EIO;
	if (nested)
		nested_vmx_setup_ctls_msrs(&vmcs_conf.nested, vmx_cap.ept);
	if (memcmp(&vmcs_config, &vmcs_conf, sizeof(struct vmcs_config)) != 0) {
		printk(KERN_ERR "kvm: CPU %d feature inconsistency!\n",
				smp_processor_id());
		return -EIO;
	}
	return 0;
}