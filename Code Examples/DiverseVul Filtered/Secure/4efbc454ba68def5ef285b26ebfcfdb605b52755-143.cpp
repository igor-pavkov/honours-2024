static void sched_feat_disable(int i)
{
	if (static_key_enabled(&sched_feat_keys[i]))
		static_key_slow_dec(&sched_feat_keys[i]);
}