void kvm_arch_exit(void)
{
	if (!boot_cpu_has(X86_FEATURE_CONSTANT_TSC))
		cpufreq_unregister_notifier(&kvmclock_cpufreq_notifier_block,
					    CPUFREQ_TRANSITION_NOTIFIER);
	kvm_x86_ops = NULL;
	kvm_mmu_module_exit();
}