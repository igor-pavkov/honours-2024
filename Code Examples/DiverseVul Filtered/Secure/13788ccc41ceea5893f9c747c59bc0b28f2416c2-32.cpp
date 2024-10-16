static int hrtimer_switch_to_hres(void)
{
	struct hrtimer_cpu_base *base = &__get_cpu_var(hrtimer_bases);
	unsigned long flags;

	if (base->hres_active)
		return 1;

	local_irq_save(flags);

	if (tick_init_highres()) {
		local_irq_restore(flags);
		return 0;
	}
	base->hres_active = 1;
	base->clock_base[CLOCK_REALTIME].resolution = KTIME_HIGH_RES;
	base->clock_base[CLOCK_MONOTONIC].resolution = KTIME_HIGH_RES;

	tick_setup_sched_timer();

	/* "Retrigger" the interrupt to get things going */
	retrigger_next_event(NULL);
	local_irq_restore(flags);
	printk(KERN_INFO "Switched to high resolution mode on CPU %d\n",
	       smp_processor_id());
	return 1;
}