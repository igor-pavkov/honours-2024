static int __cpuinit hrtimer_cpu_notify(struct notifier_block *self,
					unsigned long action, void *hcpu)
{
	long cpu = (long)hcpu;

	switch (action) {

	case CPU_UP_PREPARE:
		init_hrtimers_cpu(cpu);
		break;

#ifdef CONFIG_HOTPLUG_CPU
	case CPU_DEAD:
		clockevents_notify(CLOCK_EVT_NOTIFY_CPU_DEAD, &cpu);
		migrate_hrtimers(cpu);
		break;
#endif

	default:
		break;
	}

	return NOTIFY_OK;
}