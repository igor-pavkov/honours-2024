static int fixup_owner(u32 __user *uaddr, struct futex_q *q, int locked)
{
	int ret = 0;

	if (locked) {
		/*
		 * Got the lock. We might not be the anticipated owner if we
		 * did a lock-steal - fix up the PI-state in that case:
		 *
		 * We can safely read pi_state->owner without holding wait_lock
		 * because we now own the rt_mutex, only the owner will attempt
		 * to change it.
		 */
		if (q->pi_state->owner != current)
			ret = fixup_pi_state_owner(uaddr, q, current);
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