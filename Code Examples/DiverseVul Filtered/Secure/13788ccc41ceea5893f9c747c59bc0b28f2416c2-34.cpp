static void __devinit init_hrtimers_cpu(int cpu)
{
	struct hrtimer_cpu_base *cpu_base = &per_cpu(hrtimer_bases, cpu);
	int i;

	spin_lock_init(&cpu_base->lock);
	lockdep_set_class(&cpu_base->lock, &cpu_base->lock_key);

	for (i = 0; i < HRTIMER_MAX_CLOCK_BASES; i++)
		cpu_base->clock_base[i].cpu_base = cpu_base;

	hrtimer_init_hres(cpu_base);
}