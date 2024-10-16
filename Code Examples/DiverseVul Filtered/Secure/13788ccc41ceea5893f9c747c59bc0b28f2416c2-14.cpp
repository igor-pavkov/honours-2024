static void hrtimer_force_reprogram(struct hrtimer_cpu_base *cpu_base)
{
	int i;
	struct hrtimer_clock_base *base = cpu_base->clock_base;
	ktime_t expires;

	cpu_base->expires_next.tv64 = KTIME_MAX;

	for (i = 0; i < HRTIMER_MAX_CLOCK_BASES; i++, base++) {
		struct hrtimer *timer;

		if (!base->first)
			continue;
		timer = rb_entry(base->first, struct hrtimer, node);
		expires = ktime_sub(timer->expires, base->offset);
		if (expires.tv64 < cpu_base->expires_next.tv64)
			cpu_base->expires_next = expires;
	}

	if (cpu_base->expires_next.tv64 != KTIME_MAX)
		tick_program_event(cpu_base->expires_next, 1);
}