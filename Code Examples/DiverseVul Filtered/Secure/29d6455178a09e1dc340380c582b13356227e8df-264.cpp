static void __sched notrace preempt_schedule_common(void)
{
	do {
		preempt_disable_notrace();
		__schedule(true);
		preempt_enable_no_resched_notrace();

		/*
		 * Check again in case we missed a preemption opportunity
		 * between schedule and now.
		 */
	} while (need_resched());
}