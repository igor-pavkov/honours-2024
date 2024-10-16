static bool kvm_get_time_and_clockread(s64 *kernel_ns, cycle_t *cycle_now)
{
	struct timespec ts;

	/* checked again under seqlock below */
	if (pvclock_gtod_data.clock.vclock_mode != VCLOCK_TSC)
		return false;

	if (do_monotonic(&ts, cycle_now) != VCLOCK_TSC)
		return false;

	monotonic_to_bootbased(&ts);
	*kernel_ns = timespec_to_ns(&ts);

	return true;
}