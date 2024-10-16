static __always_inline void fire_sched_in_preempt_notifiers(struct task_struct *curr)
{
	if (static_key_false(&preempt_notifier_key))
		__fire_sched_in_preempt_notifiers(curr);
}