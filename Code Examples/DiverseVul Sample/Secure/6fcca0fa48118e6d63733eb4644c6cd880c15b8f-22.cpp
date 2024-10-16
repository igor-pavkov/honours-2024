static void psi_trigger_destroy(struct kref *ref)
{
	struct psi_trigger *t = container_of(ref, struct psi_trigger, refcount);
	struct psi_group *group = t->group;
	struct kthread_worker *kworker_to_destroy = NULL;

	if (static_branch_likely(&psi_disabled))
		return;

	/*
	 * Wakeup waiters to stop polling. Can happen if cgroup is deleted
	 * from under a polling process.
	 */
	wake_up_interruptible(&t->event_wait);

	mutex_lock(&group->trigger_lock);

	if (!list_empty(&t->node)) {
		struct psi_trigger *tmp;
		u64 period = ULLONG_MAX;

		list_del(&t->node);
		group->nr_triggers[t->state]--;
		if (!group->nr_triggers[t->state])
			group->poll_states &= ~(1 << t->state);
		/* reset min update period for the remaining triggers */
		list_for_each_entry(tmp, &group->triggers, node)
			period = min(period, div_u64(tmp->win.size,
					UPDATES_PER_WINDOW));
		group->poll_min_period = period;
		/* Destroy poll_kworker when the last trigger is destroyed */
		if (group->poll_states == 0) {
			group->polling_until = 0;
			kworker_to_destroy = rcu_dereference_protected(
					group->poll_kworker,
					lockdep_is_held(&group->trigger_lock));
			rcu_assign_pointer(group->poll_kworker, NULL);
		}
	}

	mutex_unlock(&group->trigger_lock);

	/*
	 * Wait for both *trigger_ptr from psi_trigger_replace and
	 * poll_kworker RCUs to complete their read-side critical sections
	 * before destroying the trigger and optionally the poll_kworker
	 */
	synchronize_rcu();
	/*
	 * Destroy the kworker after releasing trigger_lock to prevent a
	 * deadlock while waiting for psi_poll_work to acquire trigger_lock
	 */
	if (kworker_to_destroy) {
		/*
		 * After the RCU grace period has expired, the worker
		 * can no longer be found through group->poll_kworker.
		 * But it might have been already scheduled before
		 * that - deschedule it cleanly before destroying it.
		 */
		kthread_cancel_delayed_work_sync(&group->poll_work);
		atomic_set(&group->poll_scheduled, 0);

		kthread_destroy_worker(kworker_to_destroy);
	}
	kfree(t);
}