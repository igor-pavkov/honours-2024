void __init hrtimers_init(void)
{
	hrtimer_cpu_notify(&hrtimers_nb, (unsigned long)CPU_UP_PREPARE,
			  (void *)(long)smp_processor_id());
	register_cpu_notifier(&hrtimers_nb);
#ifdef CONFIG_HIGH_RES_TIMERS
	open_softirq(HRTIMER_SOFTIRQ, run_hrtimer_softirq, NULL);
#endif
}