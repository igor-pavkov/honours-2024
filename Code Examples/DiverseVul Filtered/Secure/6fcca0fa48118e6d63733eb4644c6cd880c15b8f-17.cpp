static void psi_schedule_poll_work(struct psi_group *group, unsigned long delay)
{
	struct kthread_worker *kworker;

	/* Do not reschedule if already scheduled */
	if (atomic_cmpxchg(&group->poll_scheduled, 0, 1) != 0)
		return;

	rcu_read_lock();

	kworker = rcu_dereference(group->poll_kworker);
	/*
	 * kworker might be NULL in case psi_trigger_destroy races with
	 * psi_task_change (hotpath) which can't use locks
	 */
	if (likely(kworker))
		kthread_queue_delayed_work(kworker, &group->poll_work, delay);
	else
		atomic_set(&group->poll_scheduled, 0);

	rcu_read_unlock();
}