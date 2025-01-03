static int pvclock_gtod_notify(struct notifier_block *nb, unsigned long unused,
			       void *priv)
{
	struct pvclock_gtod_data *gtod = &pvclock_gtod_data;
	struct timekeeper *tk = priv;

	update_pvclock_gtod(tk);

	/* disable master clock if host does not trust, or does not
	 * use, TSC clocksource
	 */
	if (gtod->clock.vclock_mode != VCLOCK_TSC &&
	    atomic_read(&kvm_guest_has_master_clock) != 0)
		queue_work(system_long_wq, &pvclock_gtod_work);

	return 0;
}