static void __cond_resched(void)
{
	__preempt_count_add(PREEMPT_ACTIVE);
	__schedule();
	__preempt_count_sub(PREEMPT_ACTIVE);
}