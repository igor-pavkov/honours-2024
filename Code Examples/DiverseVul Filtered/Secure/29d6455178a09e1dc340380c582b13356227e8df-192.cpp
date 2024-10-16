void ___might_sleep(const char *file, int line, int preempt_offset)
{
	static unsigned long prev_jiffy;	/* ratelimiting */

	rcu_sleep_check(); /* WARN_ON_ONCE() by default, no rate limit reqd. */
	if ((preempt_count_equals(preempt_offset) && !irqs_disabled() &&
	     !is_idle_task(current)) ||
	    system_state != SYSTEM_RUNNING || oops_in_progress)
		return;
	if (time_before(jiffies, prev_jiffy + HZ) && prev_jiffy)
		return;
	prev_jiffy = jiffies;

	printk(KERN_ERR
		"BUG: sleeping function called from invalid context at %s:%d\n",
			file, line);
	printk(KERN_ERR
		"in_atomic(): %d, irqs_disabled(): %d, pid: %d, name: %s\n",
			in_atomic(), irqs_disabled(),
			current->pid, current->comm);

	if (task_stack_end_corrupted(current))
		printk(KERN_EMERG "Thread overran stack, or stack corrupted\n");

	debug_show_held_locks(current);
	if (irqs_disabled())
		print_irqtrace_events(current);
#ifdef CONFIG_DEBUG_PREEMPT
	if (!preempt_count_equals(preempt_offset)) {
		pr_err("Preemption disabled at:");
		print_ip_sym(current->preempt_disable_ip);
		pr_cont("\n");
	}
#endif
	dump_stack();
}