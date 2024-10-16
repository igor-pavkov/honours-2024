static void shared_msr_update(unsigned slot, u32 msr)
{
	u64 value;
	unsigned int cpu = smp_processor_id();
	struct kvm_shared_msrs *smsr = per_cpu_ptr(shared_msrs, cpu);

	/* only read, and nobody should modify it at this time,
	 * so don't need lock */
	if (slot >= shared_msrs_global.nr) {
		printk(KERN_ERR "kvm: invalid MSR slot!");
		return;
	}
	rdmsrl_safe(msr, &value);
	smsr->values[slot].host = value;
	smsr->values[slot].curr = value;
}