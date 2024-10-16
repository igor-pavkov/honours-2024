void exit_pi_state_list(struct task_struct *curr)
{
	struct list_head *next, *head = &curr->pi_state_list;
	struct futex_pi_state *pi_state;
	struct futex_hash_bucket *hb;
	union futex_key key = FUTEX_KEY_INIT;

	if (!futex_cmpxchg_enabled)
		return;
	/*
	 * We are a ZOMBIE and nobody can enqueue itself on
	 * pi_state_list anymore, but we have to be careful
	 * versus waiters unqueueing themselves:
	 */
	raw_spin_lock_irq(&curr->pi_lock);
	while (!list_empty(head)) {
		next = head->next;
		pi_state = list_entry(next, struct futex_pi_state, list);
		key = pi_state->key;
		hb = hash_futex(&key);

		/*
		 * We can race against put_pi_state() removing itself from the
		 * list (a waiter going away). put_pi_state() will first
		 * decrement the reference count and then modify the list, so
		 * its possible to see the list entry but fail this reference
		 * acquire.
		 *
		 * In that case; drop the locks to let put_pi_state() make
		 * progress and retry the loop.
		 */
		if (!atomic_inc_not_zero(&pi_state->refcount)) {
			raw_spin_unlock_irq(&curr->pi_lock);
			cpu_relax();
			raw_spin_lock_irq(&curr->pi_lock);
			continue;
		}
		raw_spin_unlock_irq(&curr->pi_lock);

		spin_lock(&hb->lock);
		raw_spin_lock_irq(&pi_state->pi_mutex.wait_lock);
		raw_spin_lock(&curr->pi_lock);
		/*
		 * We dropped the pi-lock, so re-check whether this
		 * task still owns the PI-state:
		 */
		if (head->next != next) {
			/* retain curr->pi_lock for the loop invariant */
			raw_spin_unlock(&pi_state->pi_mutex.wait_lock);
			spin_unlock(&hb->lock);
			put_pi_state(pi_state);
			continue;
		}

		WARN_ON(pi_state->owner != curr);
		WARN_ON(list_empty(&pi_state->list));
		list_del_init(&pi_state->list);
		pi_state->owner = NULL;

		raw_spin_unlock(&curr->pi_lock);
		raw_spin_unlock_irq(&pi_state->pi_mutex.wait_lock);
		spin_unlock(&hb->lock);

		rt_mutex_futex_unlock(&pi_state->pi_mutex);
		put_pi_state(pi_state);

		raw_spin_lock_irq(&curr->pi_lock);
	}
	raw_spin_unlock_irq(&curr->pi_lock);
}