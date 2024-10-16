static int attach_to_pi_state(u32 __user *uaddr, u32 uval,
			      struct futex_pi_state *pi_state,
			      struct futex_pi_state **ps)
{
	pid_t pid = uval & FUTEX_TID_MASK;
	u32 uval2;
	int ret;

	/*
	 * Userspace might have messed up non-PI and PI futexes [3]
	 */
	if (unlikely(!pi_state))
		return -EINVAL;

	/*
	 * We get here with hb->lock held, and having found a
	 * futex_top_waiter(). This means that futex_lock_pi() of said futex_q
	 * has dropped the hb->lock in between queue_me() and unqueue_me_pi(),
	 * which in turn means that futex_lock_pi() still has a reference on
	 * our pi_state.
	 *
	 * The waiter holding a reference on @pi_state also protects against
	 * the unlocked put_pi_state() in futex_unlock_pi(), futex_lock_pi()
	 * and futex_wait_requeue_pi() as it cannot go to 0 and consequently
	 * free pi_state before we can take a reference ourselves.
	 */
	WARN_ON(!atomic_read(&pi_state->refcount));

	/*
	 * Now that we have a pi_state, we can acquire wait_lock
	 * and do the state validation.
	 */
	raw_spin_lock_irq(&pi_state->pi_mutex.wait_lock);

	/*
	 * Since {uval, pi_state} is serialized by wait_lock, and our current
	 * uval was read without holding it, it can have changed. Verify it
	 * still is what we expect it to be, otherwise retry the entire
	 * operation.
	 */
	if (get_futex_value_locked(&uval2, uaddr))
		goto out_efault;

	if (uval != uval2)
		goto out_eagain;

	/*
	 * Handle the owner died case:
	 */
	if (uval & FUTEX_OWNER_DIED) {
		/*
		 * exit_pi_state_list sets owner to NULL and wakes the
		 * topmost waiter. The task which acquires the
		 * pi_state->rt_mutex will fixup owner.
		 */
		if (!pi_state->owner) {
			/*
			 * No pi state owner, but the user space TID
			 * is not 0. Inconsistent state. [5]
			 */
			if (pid)
				goto out_einval;
			/*
			 * Take a ref on the state and return success. [4]
			 */
			goto out_attach;
		}

		/*
		 * If TID is 0, then either the dying owner has not
		 * yet executed exit_pi_state_list() or some waiter
		 * acquired the rtmutex in the pi state, but did not
		 * yet fixup the TID in user space.
		 *
		 * Take a ref on the state and return success. [6]
		 */
		if (!pid)
			goto out_attach;
	} else {
		/*
		 * If the owner died bit is not set, then the pi_state
		 * must have an owner. [7]
		 */
		if (!pi_state->owner)
			goto out_einval;
	}

	/*
	 * Bail out if user space manipulated the futex value. If pi
	 * state exists then the owner TID must be the same as the
	 * user space TID. [9/10]
	 */
	if (pid != task_pid_vnr(pi_state->owner))
		goto out_einval;

out_attach:
	get_pi_state(pi_state);
	raw_spin_unlock_irq(&pi_state->pi_mutex.wait_lock);
	*ps = pi_state;
	return 0;

out_einval:
	ret = -EINVAL;
	goto out_error;

out_eagain:
	ret = -EAGAIN;
	goto out_error;

out_efault:
	ret = -EFAULT;
	goto out_error;

out_error:
	raw_spin_unlock_irq(&pi_state->pi_mutex.wait_lock);
	return ret;
}