void kvm_arch_exit(void)
{
	perf_unregister_guest_info_callbacks(&kvm_guest_cbs);

	if (!boot_cpu_has(X86_FEATURE_CONSTANT_TSC))
		cpufreq_unregister_notifier(&kvmclock_cpufreq_notifier_block,
					    CPUFREQ_TRANSITION_NOTIFIER);
	unregister_hotcpu_notifier(&kvmclock_cpu_notifier_block);
#ifdef CONFIG_X86_64
	pvclock_gtod_unregister_notifier(&pvclock_gtod_notifier);
#endif
	kvm_x86_ops = NULL;
	kvm_mmu_module_exit();
	free_percpu(shared_msrs);
}