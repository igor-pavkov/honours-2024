static void kvm_write_guest_time(struct kvm_vcpu *v)
{
	struct timespec ts;
	unsigned long flags;
	struct kvm_vcpu_arch *vcpu = &v->arch;
	void *shared_kaddr;
	unsigned long this_tsc_khz;

	if ((!vcpu->time_page))
		return;

	this_tsc_khz = get_cpu_var(cpu_tsc_khz);
	if (unlikely(vcpu->hv_clock_tsc_khz != this_tsc_khz)) {
		kvm_set_time_scale(this_tsc_khz, &vcpu->hv_clock);
		vcpu->hv_clock_tsc_khz = this_tsc_khz;
	}
	put_cpu_var(cpu_tsc_khz);

	/* Keep irq disabled to prevent changes to the clock */
	local_irq_save(flags);
	kvm_get_msr(v, MSR_IA32_TIME_STAMP_COUNTER,
			  &vcpu->hv_clock.tsc_timestamp);
	ktime_get_ts(&ts);
	local_irq_restore(flags);

	/* With all the info we got, fill in the values */

	vcpu->hv_clock.system_time = ts.tv_nsec +
				     (NSEC_PER_SEC * (u64)ts.tv_sec);
	/*
	 * The interface expects us to write an even number signaling that the
	 * update is finished. Since the guest won't see the intermediate
	 * state, we just increase by 2 at the end.
	 */
	vcpu->hv_clock.version += 2;

	shared_kaddr = kmap_atomic(vcpu->time_page, KM_USER0);

	memcpy(shared_kaddr + vcpu->time_offset, &vcpu->hv_clock,
	       sizeof(vcpu->hv_clock));

	kunmap_atomic(shared_kaddr, KM_USER0);

	mark_page_dirty(v->kvm, vcpu->time >> PAGE_SHIFT);
}