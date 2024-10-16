static int fixup_owner(u32 __user *uaddr, struct futex_q *q, int locked)
{
	int ret = 0;

	if (locked) {
		/*
		 * Got the lock. We might not be the anticipated owner if we
		 * did a lock-steal - fix up the PI-state in that case:
		 *
		 * Speculative pi_state->owner read (we don't hold wait_lock);
		 * since we own the lock pi_state->owner == current is the
		 * stable state, anything else needs more attention.
		 */
		if (q->pi_state->owner != current)
			ret = fixup_pi_state_owner(uaddr, q, current);
		goto out;
	}

	/*
	 * If we didn't get the lock; check if anybody stole it from us. In
	 * that case, we need to fix up the uval to point to them instead of
	 * us, otherwise bad things happen. [10]
	 *
	 * Another speculative read; pi_state->owner == current is unstable
	 * but needs our attention.
	 */
	if (q->pi_state->owner == current) {
		ret = fixup_pi_state_owner(uaddr, q, NULL);
		goto out;
	}

	/*
	 * Paranoia check. If we did not take the lock, then we should not be
	 * the owner of the rt_mutex.
	 */
	if (rt_mutex_owner(&q->pi_state->pi_mutex) == current) {
		printk(KERN_ERR "fixup_owner: ret = %d pi-mutex: %p "
				"pi-state %p\n", ret,
				q->pi_state->pi_mutex.owner,
				q->pi_state->owner);
	}

out:
	return ret ? ret : locked;
}