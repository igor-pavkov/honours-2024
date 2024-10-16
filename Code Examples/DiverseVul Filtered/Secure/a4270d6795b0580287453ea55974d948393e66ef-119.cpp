static void netstamp_clear(struct work_struct *work)
{
	int deferred = atomic_xchg(&netstamp_needed_deferred, 0);
	int wanted;

	wanted = atomic_add_return(deferred, &netstamp_wanted);
	if (wanted > 0)
		static_branch_enable(&netstamp_needed_key);
	else
		static_branch_disable(&netstamp_needed_key);
}