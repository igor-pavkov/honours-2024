static int kvmclock_cpufreq_notifier(struct notifier_block *nb, unsigned long val,
				     void *data)
{
	struct cpufreq_freqs *freq = data;
	struct kvm *kvm;
	struct kvm_vcpu *vcpu;
	int i, send_ipi = 0;

	if (!ref_freq)
		ref_freq = freq->old;

	if (val == CPUFREQ_PRECHANGE && freq->old > freq->new)
		return 0;
	if (val == CPUFREQ_POSTCHANGE && freq->old < freq->new)
		return 0;
	per_cpu(cpu_tsc_khz, freq->cpu) = cpufreq_scale(tsc_khz_ref, ref_freq, freq->new);

	spin_lock(&kvm_lock);
	list_for_each_entry(kvm, &vm_list, vm_list) {
		for (i = 0; i < KVM_MAX_VCPUS; ++i) {
			vcpu = kvm->vcpus[i];
			if (!vcpu)
				continue;
			if (vcpu->cpu != freq->cpu)
				continue;
			if (!kvm_request_guest_time_update(vcpu))
				continue;
			if (vcpu->cpu != smp_processor_id())
				send_ipi++;
		}
	}
	spin_unlock(&kvm_lock);

	if (freq->old < freq->new && send_ipi) {
		/*
		 * We upscale the frequency.  Must make the guest
		 * doesn't see old kvmclock values while running with
		 * the new frequency, otherwise we risk the guest sees
		 * time go backwards.
		 *
		 * In case we update the frequency for another cpu
		 * (which might be in guest context) send an interrupt
		 * to kick the cpu out of guest context.  Next time
		 * guest context is entered kvmclock will be updated,
		 * so the guest will not see stale values.
		 */
		smp_call_function_single(freq->cpu, bounce_off, NULL, 1);
	}
	return 0;
}