static int fixup_pi_state_owner(u32 __user *uaddr, struct futex_q *q,
				struct task_struct *argowner)
{
	struct futex_pi_state *pi_state = q->pi_state;
	u32 uval, uninitialized_var(curval), newval;
	struct task_struct *oldowner, *newowner;
	u32 newtid;
	int ret;

	lockdep_assert_held(q->lock_ptr);

	raw_spin_lock_irq(&pi_state->pi_mutex.wait_lock);

	oldowner = pi_state->owner;
	/* Owner died? */
	if (!pi_state->owner)
		newtid |= FUTEX_OWNER_DIED;

	/*
	 * We are here because either:
	 *
	 *  - we stole the lock and pi_state->owner needs updating to reflect
	 *    that (@argowner == current),
	 *
	 * or:
	 *
	 *  - someone stole our lock and we need to fix things to point to the
	 *    new owner (@argowner == NULL).
	 *
	 * Either way, we have to replace the TID in the user space variable.
	 * This must be atomic as we have to preserve the owner died bit here.
	 *
	 * Note: We write the user space value _before_ changing the pi_state
	 * because we can fault here. Imagine swapped out pages or a fork
	 * that marked all the anonymous memory readonly for cow.
	 *
	 * Modifying pi_state _before_ the user space value would leave the
	 * pi_state in an inconsistent state when we fault here, because we
	 * need to drop the locks to handle the fault. This might be observed
	 * in the PID check in lookup_pi_state.
	 */
retry:
	if (!argowner) {
		if (oldowner != current) {
			/*
			 * We raced against a concurrent self; things are
			 * already fixed up. Nothing to do.
			 */
			ret = 0;
			goto out_unlock;
		}

		if (__rt_mutex_futex_trylock(&pi_state->pi_mutex)) {
			/* We got the lock after all, nothing to fix. */
			ret = 0;
			goto out_unlock;
		}

		/*
		 * Since we just failed the trylock; there must be an owner.
		 */
		newowner = rt_mutex_owner(&pi_state->pi_mutex);
		BUG_ON(!newowner);
	} else {
		WARN_ON_ONCE(argowner != current);
		if (oldowner == current) {
			/*
			 * We raced against a concurrent self; things are
			 * already fixed up. Nothing to do.
			 */
			ret = 0;
			goto out_unlock;
		}
		newowner = argowner;
	}

	newtid = task_pid_vnr(newowner) | FUTEX_WAITERS;

	if (get_futex_value_locked(&uval, uaddr))
		goto handle_fault;

	for (;;) {
		newval = (uval & FUTEX_OWNER_DIED) | newtid;

		if (cmpxchg_futex_value_locked(&curval, uaddr, uval, newval))
			goto handle_fault;
		if (curval == uval)
			break;
		uval = curval;
	}

	/*
	 * We fixed up user space. Now we need to fix the pi_state
	 * itself.
	 */
	if (pi_state->owner != NULL) {
		raw_spin_lock(&pi_state->owner->pi_lock);
		WARN_ON(list_empty(&pi_state->list));
		list_del_init(&pi_state->list);
		raw_spin_unlock(&pi_state->owner->pi_lock);
	}

	pi_state->owner = newowner;

	raw_spin_lock(&newowner->pi_lock);
	WARN_ON(!list_empty(&pi_state->list));
	list_add(&pi_state->list, &newowner->pi_state_list);
	raw_spin_unlock(&newowner->pi_lock);
	raw_spin_unlock_irq(&pi_state->pi_mutex.wait_lock);

	return 0;

	/*
	 * To handle the page fault we need to drop the locks here. That gives
	 * the other task (either the highest priority waiter itself or the
	 * task which stole the rtmutex) the chance to try the fixup of the
	 * pi_state. So once we are back from handling the fault we need to
	 * check the pi_state after reacquiring the locks and before trying to
	 * do another fixup. When the fixup has been done already we simply
	 * return.
	 *
	 * Note: we hold both hb->lock and pi_mutex->wait_lock. We can safely
	 * drop hb->lock since the caller owns the hb -> futex_q relation.
	 * Dropping the pi_mutex->wait_lock requires the state revalidate.
	 */
handle_fault:
	raw_spin_unlock_irq(&pi_state->pi_mutex.wait_lock);
	spin_unlock(q->lock_ptr);

	ret = fault_in_user_writeable(uaddr);

	spin_lock(q->lock_ptr);
	raw_spin_lock_irq(&pi_state->pi_mutex.wait_lock);

	/*
	 * Check if someone else fixed it for us:
	 */
	if (pi_state->owner != oldowner) {
		ret = 0;
		goto out_unlock;
	}

	if (ret)
		goto out_unlock;

	goto retry;

out_unlock:
	raw_spin_unlock_irq(&pi_state->pi_mutex.wait_lock);
	return ret;
}