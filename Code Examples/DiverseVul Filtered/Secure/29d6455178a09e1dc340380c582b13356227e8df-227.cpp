void preempt_notifier_dec(void)
{
	static_key_slow_dec(&preempt_notifier_key);
}