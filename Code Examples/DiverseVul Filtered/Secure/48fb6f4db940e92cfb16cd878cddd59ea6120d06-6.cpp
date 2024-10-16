static int futex_unlock_pi(u32 __user *uaddr, unsigned int flags)
{
	u32 uninitialized_var(curval), uval, vpid = task_pid_vnr(current);
	union futex_key key = FUTEX_KEY_INIT;
	struct futex_hash_bucket *hb;
	struct futex_q *top_waiter;
	int ret;

retry:
	if (get_user(uval, uaddr))
		return -EFAULT;
	/*
	 * We release only a lock we actually own:
	 */
	if ((uval & FUTEX_TID_MASK) != vpid)
		return -EPERM;

	ret = get_futex_key(uaddr, flags & FLAGS_SHARED, &key, VERIFY_WRITE);
	if (ret)
		return ret;

	hb = hash_futex(&key);
	spin_lock(&hb->lock);

	/*
	 * Check waiters first. We do not trust user space values at
	 * all and we at least want to know if user space fiddled
	 * with the futex value instead of blindly unlocking.
	 */
	top_waiter = futex_top_waiter(hb, &key);
	if (top_waiter) {
		struct futex_pi_state *pi_state = top_waiter->pi_state;

		ret = -EINVAL;
		if (!pi_state)
			goto out_unlock;

		/*
		 * If current does not own the pi_state then the futex is
		 * inconsistent and user space fiddled with the futex value.
		 */
		if (pi_state->owner != current)
			goto out_unlock;

		get_pi_state(pi_state);
		/*
		 * By taking wait_lock while still holding hb->lock, we ensure
		 * there is no point where we hold neither; and therefore
		 * wake_futex_pi() must observe a state consistent with what we
		 * observed.
		 */
		raw_spin_lock_irq(&pi_state->pi_mutex.wait_lock);
		spin_unlock(&hb->lock);

		ret = wake_futex_pi(uaddr, uval, pi_state);

		put_pi_state(pi_state);

		/*
		 * Success, we're done! No tricky corner cases.
		 */
		if (!ret)
			goto out_putkey;
		/*
		 * The atomic access to the futex value generated a
		 * pagefault, so retry the user-access and the wakeup:
		 */
		if (ret == -EFAULT)
			goto pi_faulted;
		/*
		 * A unconditional UNLOCK_PI op raced against a waiter
		 * setting the FUTEX_WAITERS bit. Try again.
		 */
		if (ret == -EAGAIN) {
			put_futex_key(&key);
			goto retry;
		}
		/*
		 * wake_futex_pi has detected invalid state. Tell user
		 * space.
		 */
		goto out_putkey;
	}

	/*
	 * We have no kernel internal state, i.e. no waiters in the
	 * kernel. Waiters which are about to queue themselves are stuck
	 * on hb->lock. So we can safely ignore them. We do neither
	 * preserve the WAITERS bit not the OWNER_DIED one. We are the
	 * owner.
	 */
	if (cmpxchg_futex_value_locked(&curval, uaddr, uval, 0)) {
		spin_unlock(&hb->lock);
		goto pi_faulted;
	}

	/*
	 * If uval has changed, let user space handle it.
	 */
	ret = (curval == uval) ? 0 : -EAGAIN;

out_unlock:
	spin_unlock(&hb->lock);
out_putkey:
	put_futex_key(&key);
	return ret;

pi_faulted:
	put_futex_key(&key);

	ret = fault_in_user_writeable(uaddr);
	if (!ret)
		goto retry;

	return ret;
}