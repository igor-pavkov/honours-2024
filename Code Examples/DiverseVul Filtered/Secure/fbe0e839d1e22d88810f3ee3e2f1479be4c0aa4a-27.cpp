static int wake_futex_pi(u32 __user *uaddr, u32 uval, struct futex_pi_state *pi_state)
{
	u32 uninitialized_var(curval), newval;
	struct task_struct *new_owner;
	bool postunlock = false;
	DEFINE_WAKE_Q(wake_q);
	int ret = 0;

	new_owner = rt_mutex_next_owner(&pi_state->pi_mutex);
	if (WARN_ON_ONCE(!new_owner)) {
		/*
		 * As per the comment in futex_unlock_pi() this should not happen.
		 *
		 * When this happens, give up our locks and try again, giving
		 * the futex_lock_pi() instance time to complete, either by
		 * waiting on the rtmutex or removing itself from the futex
		 * queue.
		 */
		ret = -EAGAIN;
		goto out_unlock;
	}

	/*
	 * We pass it to the next owner. The WAITERS bit is always kept
	 * enabled while there is PI state around. We cleanup the owner
	 * died bit, because we are the owner.
	 */
	newval = FUTEX_WAITERS | task_pid_vnr(new_owner);

	if (unlikely(should_fail_futex(true)))
		ret = -EFAULT;

	if (cmpxchg_futex_value_locked(&curval, uaddr, uval, newval)) {
		ret = -EFAULT;

	} else if (curval != uval) {
		/*
		 * If a unconditional UNLOCK_PI operation (user space did not
		 * try the TID->0 transition) raced with a waiter setting the
		 * FUTEX_WAITERS flag between get_user() and locking the hash
		 * bucket lock, retry the operation.
		 */
		if ((FUTEX_TID_MASK & curval) == uval)
			ret = -EAGAIN;
		else
			ret = -EINVAL;
	}

	if (ret)
		goto out_unlock;

	/*
	 * This is a point of no return; once we modify the uval there is no
	 * going back and subsequent operations must not fail.
	 */

	raw_spin_lock(&pi_state->owner->pi_lock);
	WARN_ON(list_empty(&pi_state->list));
	list_del_init(&pi_state->list);
	raw_spin_unlock(&pi_state->owner->pi_lock);

	raw_spin_lock(&new_owner->pi_lock);
	WARN_ON(!list_empty(&pi_state->list));
	list_add(&pi_state->list, &new_owner->pi_state_list);
	pi_state->owner = new_owner;
	raw_spin_unlock(&new_owner->pi_lock);

	postunlock = __rt_mutex_futex_unlock(&pi_state->pi_mutex, &wake_q);

out_unlock:
	raw_spin_unlock_irq(&pi_state->pi_mutex.wait_lock);

	if (postunlock)
		rt_mutex_postunlock(&wake_q);

	return ret;
}