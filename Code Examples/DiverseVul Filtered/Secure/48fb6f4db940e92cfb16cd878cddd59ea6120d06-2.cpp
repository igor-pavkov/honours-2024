static int futex_lock_pi(u32 __user *uaddr, unsigned int flags,
			 ktime_t *time, int trylock)
{
	struct hrtimer_sleeper timeout, *to = NULL;
	struct futex_pi_state *pi_state = NULL;
	struct rt_mutex_waiter rt_waiter;
	struct futex_hash_bucket *hb;
	struct futex_q q = futex_q_init;
	int res, ret;

	if (refill_pi_state_cache())
		return -ENOMEM;

	if (time) {
		to = &timeout;
		hrtimer_init_on_stack(&to->timer, CLOCK_REALTIME,
				      HRTIMER_MODE_ABS);
		hrtimer_init_sleeper(to, current);
		hrtimer_set_expires(&to->timer, *time);
	}

retry:
	ret = get_futex_key(uaddr, flags & FLAGS_SHARED, &q.key, VERIFY_WRITE);
	if (unlikely(ret != 0))
		goto out;

retry_private:
	hb = queue_lock(&q);

	ret = futex_lock_pi_atomic(uaddr, hb, &q.key, &q.pi_state, current, 0);
	if (unlikely(ret)) {
		/*
		 * Atomic work succeeded and we got the lock,
		 * or failed. Either way, we do _not_ block.
		 */
		switch (ret) {
		case 1:
			/* We got the lock. */
			ret = 0;
			goto out_unlock_put_key;
		case -EFAULT:
			goto uaddr_faulted;
		case -EAGAIN:
			/*
			 * Two reasons for this:
			 * - Task is exiting and we just wait for the
			 *   exit to complete.
			 * - The user space value changed.
			 */
			queue_unlock(hb);
			put_futex_key(&q.key);
			cond_resched();
			goto retry;
		default:
			goto out_unlock_put_key;
		}
	}

	WARN_ON(!q.pi_state);

	/*
	 * Only actually queue now that the atomic ops are done:
	 */
	__queue_me(&q, hb);

	if (trylock) {
		ret = rt_mutex_futex_trylock(&q.pi_state->pi_mutex);
		/* Fixup the trylock return value: */
		ret = ret ? 0 : -EWOULDBLOCK;
		goto no_block;
	}

	rt_mutex_init_waiter(&rt_waiter);

	/*
	 * On PREEMPT_RT_FULL, when hb->lock becomes an rt_mutex, we must not
	 * hold it while doing rt_mutex_start_proxy(), because then it will
	 * include hb->lock in the blocking chain, even through we'll not in
	 * fact hold it while blocking. This will lead it to report -EDEADLK
	 * and BUG when futex_unlock_pi() interleaves with this.
	 *
	 * Therefore acquire wait_lock while holding hb->lock, but drop the
	 * latter before calling rt_mutex_start_proxy_lock(). This still fully
	 * serializes against futex_unlock_pi() as that does the exact same
	 * lock handoff sequence.
	 */
	raw_spin_lock_irq(&q.pi_state->pi_mutex.wait_lock);
	spin_unlock(q.lock_ptr);
	ret = __rt_mutex_start_proxy_lock(&q.pi_state->pi_mutex, &rt_waiter, current);
	raw_spin_unlock_irq(&q.pi_state->pi_mutex.wait_lock);

	if (ret) {
		if (ret == 1)
			ret = 0;

		spin_lock(q.lock_ptr);
		goto no_block;
	}


	if (unlikely(to))
		hrtimer_start_expires(&to->timer, HRTIMER_MODE_ABS);

	ret = rt_mutex_wait_proxy_lock(&q.pi_state->pi_mutex, to, &rt_waiter);

	spin_lock(q.lock_ptr);
	/*
	 * If we failed to acquire the lock (signal/timeout), we must
	 * first acquire the hb->lock before removing the lock from the
	 * rt_mutex waitqueue, such that we can keep the hb and rt_mutex
	 * wait lists consistent.
	 *
	 * In particular; it is important that futex_unlock_pi() can not
	 * observe this inconsistency.
	 */
	if (ret && !rt_mutex_cleanup_proxy_lock(&q.pi_state->pi_mutex, &rt_waiter))
		ret = 0;

no_block:
	/*
	 * Fixup the pi_state owner and possibly acquire the lock if we
	 * haven't already.
	 */
	res = fixup_owner(uaddr, &q, !ret);
	/*
	 * If fixup_owner() returned an error, proprogate that.  If it acquired
	 * the lock, clear our -ETIMEDOUT or -EINTR.
	 */
	if (res)
		ret = (res < 0) ? res : 0;

	/*
	 * If fixup_owner() faulted and was unable to handle the fault, unlock
	 * it and return the fault to userspace.
	 */
	if (ret && (rt_mutex_owner(&q.pi_state->pi_mutex) == current)) {
		pi_state = q.pi_state;
		get_pi_state(pi_state);
	}

	/* Unqueue and drop the lock */
	unqueue_me_pi(&q);

	if (pi_state) {
		rt_mutex_futex_unlock(&pi_state->pi_mutex);
		put_pi_state(pi_state);
	}

	goto out_put_key;

out_unlock_put_key:
	queue_unlock(hb);

out_put_key:
	put_futex_key(&q.key);
out:
	if (to) {
		hrtimer_cancel(&to->timer);
		destroy_hrtimer_on_stack(&to->timer);
	}
	return ret != -EINTR ? ret : -ERESTARTNOINTR;

uaddr_faulted:
	queue_unlock(hb);

	ret = fault_in_user_writeable(uaddr);
	if (ret)
		goto out_put_key;

	if (!(flags & FLAGS_SHARED))
		goto retry_private;

	put_futex_key(&q.key);
	goto retry;
}