static void function_trace_probe_call(unsigned long ip, unsigned long parent_ip,
				      struct ftrace_ops *op, struct pt_regs *pt_regs)
{
	struct ftrace_func_probe *entry;
	struct hlist_head *hhd;
	unsigned long key;

	key = hash_long(ip, FTRACE_HASH_BITS);

	hhd = &ftrace_func_hash[key];

	if (hlist_empty(hhd))
		return;

	/*
	 * Disable preemption for these calls to prevent a RCU grace
	 * period. This syncs the hash iteration and freeing of items
	 * on the hash. rcu_read_lock is too dangerous here.
	 */
	preempt_disable_notrace();
	hlist_for_each_entry_rcu(entry, hhd, node) {
		if (entry->ip == ip)
			entry->ops->func(ip, parent_ip, &entry->data);
	}
	preempt_enable_notrace();
}