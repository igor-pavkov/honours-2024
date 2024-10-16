static int futex_requeue(u32 __user *uaddr1, unsigned int flags,
			 u32 __user *uaddr2, int nr_wake, int nr_requeue,
			 u32 *cmpval, int requeue_pi)
{
	union futex_key key1 = FUTEX_KEY_INIT, key2 = FUTEX_KEY_INIT;
	int drop_count = 0, task_count = 0, ret;
	struct futex_pi_state *pi_state = NULL;
	struct futex_hash_bucket *hb1, *hb2;
	struct futex_q *this, *next;
	DEFINE_WAKE_Q(wake_q);

	if (requeue_pi) {
		/*
		 * Requeue PI only works on two distinct uaddrs. This
		 * check is only valid for private futexes. See below.
		 */
		if (uaddr1 == uaddr2)
			return -EINVAL;

		/*
		 * requeue_pi requires a pi_state, try to allocate it now
		 * without any locks in case it fails.
		 */
		if (refill_pi_state_cache())
			return -ENOMEM;
		/*
		 * requeue_pi must wake as many tasks as it can, up to nr_wake
		 * + nr_requeue, since it acquires the rt_mutex prior to
		 * returning to userspace, so as to not leave the rt_mutex with
		 * waiters and no owner.  However, second and third wake-ups
		 * cannot be predicted as they involve race conditions with the
		 * first wake and a fault while looking up the pi_state.  Both
		 * pthread_cond_signal() and pthread_cond_broadcast() should
		 * use nr_wake=1.
		 */
		if (nr_wake != 1)
			return -EINVAL;
	}

retry:
	ret = get_futex_key(uaddr1, flags & FLAGS_SHARED, &key1, VERIFY_READ);
	if (unlikely(ret != 0))
		goto out;
	ret = get_futex_key(uaddr2, flags & FLAGS_SHARED, &key2,
			    requeue_pi ? VERIFY_WRITE : VERIFY_READ);
	if (unlikely(ret != 0))
		goto out_put_key1;

	/*
	 * The check above which compares uaddrs is not sufficient for
	 * shared futexes. We need to compare the keys:
	 */
	if (requeue_pi && match_futex(&key1, &key2)) {
		ret = -EINVAL;
		goto out_put_keys;
	}

	hb1 = hash_futex(&key1);
	hb2 = hash_futex(&key2);

retry_private:
	hb_waiters_inc(hb2);
	double_lock_hb(hb1, hb2);

	if (likely(cmpval != NULL)) {
		u32 curval;

		ret = get_futex_value_locked(&curval, uaddr1);

		if (unlikely(ret)) {
			double_unlock_hb(hb1, hb2);
			hb_waiters_dec(hb2);

			ret = get_user(curval, uaddr1);
			if (ret)
				goto out_put_keys;

			if (!(flags & FLAGS_SHARED))
				goto retry_private;

			put_futex_key(&key2);
			put_futex_key(&key1);
			goto retry;
		}
		if (curval != *cmpval) {
			ret = -EAGAIN;
			goto out_unlock;
		}
	}

	if (requeue_pi && (task_count - nr_wake < nr_requeue)) {
		/*
		 * Attempt to acquire uaddr2 and wake the top waiter. If we
		 * intend to requeue waiters, force setting the FUTEX_WAITERS
		 * bit.  We force this here where we are able to easily handle
		 * faults rather in the requeue loop below.
		 */
		ret = futex_proxy_trylock_atomic(uaddr2, hb1, hb2, &key1,
						 &key2, &pi_state, nr_requeue);

		/*
		 * At this point the top_waiter has either taken uaddr2 or is
		 * waiting on it.  If the former, then the pi_state will not
		 * exist yet, look it up one more time to ensure we have a
		 * reference to it. If the lock was taken, ret contains the
		 * vpid of the top waiter task.
		 * If the lock was not taken, we have pi_state and an initial
		 * refcount on it. In case of an error we have nothing.
		 */
		if (ret > 0) {
			WARN_ON(pi_state);
			drop_count++;
			task_count++;
			/*
			 * If we acquired the lock, then the user space value
			 * of uaddr2 should be vpid. It cannot be changed by
			 * the top waiter as it is blocked on hb2 lock if it
			 * tries to do so. If something fiddled with it behind
			 * our back the pi state lookup might unearth it. So
			 * we rather use the known value than rereading and
			 * handing potential crap to lookup_pi_state.
			 *
			 * If that call succeeds then we have pi_state and an
			 * initial refcount on it.
			 */
			ret = lookup_pi_state(uaddr2, ret, hb2, &key2, &pi_state);
		}

		switch (ret) {
		case 0:
			/* We hold a reference on the pi state. */
			break;

			/* If the above failed, then pi_state is NULL */
		case -EFAULT:
			double_unlock_hb(hb1, hb2);
			hb_waiters_dec(hb2);
			put_futex_key(&key2);
			put_futex_key(&key1);
			ret = fault_in_user_writeable(uaddr2);
			if (!ret)
				goto retry;
			goto out;
		case -EAGAIN:
			/*
			 * Two reasons for this:
			 * - Owner is exiting and we just wait for the
			 *   exit to complete.
			 * - The user space value changed.
			 */
			double_unlock_hb(hb1, hb2);
			hb_waiters_dec(hb2);
			put_futex_key(&key2);
			put_futex_key(&key1);
			cond_resched();
			goto retry;
		default:
			goto out_unlock;
		}
	}

	plist_for_each_entry_safe(this, next, &hb1->chain, list) {
		if (task_count - nr_wake >= nr_requeue)
			break;

		if (!match_futex(&this->key, &key1))
			continue;

		/*
		 * FUTEX_WAIT_REQEUE_PI and FUTEX_CMP_REQUEUE_PI should always
		 * be paired with each other and no other futex ops.
		 *
		 * We should never be requeueing a futex_q with a pi_state,
		 * which is awaiting a futex_unlock_pi().
		 */
		if ((requeue_pi && !this->rt_waiter) ||
		    (!requeue_pi && this->rt_waiter) ||
		    this->pi_state) {
			ret = -EINVAL;
			break;
		}

		/*
		 * Wake nr_wake waiters.  For requeue_pi, if we acquired the
		 * lock, we already woke the top_waiter.  If not, it will be
		 * woken by futex_unlock_pi().
		 */
		if (++task_count <= nr_wake && !requeue_pi) {
			mark_wake_futex(&wake_q, this);
			continue;
		}

		/* Ensure we requeue to the expected futex for requeue_pi. */
		if (requeue_pi && !match_futex(this->requeue_pi_key, &key2)) {
			ret = -EINVAL;
			break;
		}

		/*
		 * Requeue nr_requeue waiters and possibly one more in the case
		 * of requeue_pi if we couldn't acquire the lock atomically.
		 */
		if (requeue_pi) {
			/*
			 * Prepare the waiter to take the rt_mutex. Take a
			 * refcount on the pi_state and store the pointer in
			 * the futex_q object of the waiter.
			 */
			get_pi_state(pi_state);
			this->pi_state = pi_state;
			ret = rt_mutex_start_proxy_lock(&pi_state->pi_mutex,
							this->rt_waiter,
							this->task);
			if (ret == 1) {
				/*
				 * We got the lock. We do neither drop the
				 * refcount on pi_state nor clear
				 * this->pi_state because the waiter needs the
				 * pi_state for cleaning up the user space
				 * value. It will drop the refcount after
				 * doing so.
				 */
				requeue_pi_wake_futex(this, &key2, hb2);
				drop_count++;
				continue;
			} else if (ret) {
				/*
				 * rt_mutex_start_proxy_lock() detected a
				 * potential deadlock when we tried to queue
				 * that waiter. Drop the pi_state reference
				 * which we took above and remove the pointer
				 * to the state from the waiters futex_q
				 * object.
				 */
				this->pi_state = NULL;
				put_pi_state(pi_state);
				/*
				 * We stop queueing more waiters and let user
				 * space deal with the mess.
				 */
				break;
			}
		}
		requeue_futex(this, hb1, hb2, &key2);
		drop_count++;
	}

	/*
	 * We took an extra initial reference to the pi_state either
	 * in futex_proxy_trylock_atomic() or in lookup_pi_state(). We
	 * need to drop it here again.
	 */
	put_pi_state(pi_state);

out_unlock:
	double_unlock_hb(hb1, hb2);
	wake_up_q(&wake_q);
	hb_waiters_dec(hb2);

	/*
	 * drop_futex_key_refs() must be called outside the spinlocks. During
	 * the requeue we moved futex_q's from the hash bucket at key1 to the
	 * one at key2 and updated their key pointer.  We no longer need to
	 * hold the references to key1.
	 */
	while (--drop_count >= 0)
		drop_futex_key_refs(&key1);

out_put_keys:
	put_futex_key(&key2);
out_put_key1:
	put_futex_key(&key1);
out:
	return ret ? ret : task_count;
}