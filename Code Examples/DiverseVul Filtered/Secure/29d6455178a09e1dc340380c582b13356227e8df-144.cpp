void preempt_notifier_inc(void)
{
	static_key_slow_inc(&preempt_notifier_key);
}