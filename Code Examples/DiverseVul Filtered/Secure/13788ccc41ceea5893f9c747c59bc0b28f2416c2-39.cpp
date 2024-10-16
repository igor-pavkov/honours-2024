long hrtimer_nanosleep(struct timespec *rqtp, struct timespec __user *rmtp,
		       const enum hrtimer_mode mode, const clockid_t clockid)
{
	struct restart_block *restart;
	struct hrtimer_sleeper t;
	struct timespec tu;
	ktime_t rem;

	hrtimer_init(&t.timer, clockid, mode);
	t.timer.expires = timespec_to_ktime(*rqtp);
	if (do_nanosleep(&t, mode))
		return 0;

	/* Absolute timers do not update the rmtp value and restart: */
	if (mode == HRTIMER_MODE_ABS)
		return -ERESTARTNOHAND;

	if (rmtp) {
		rem = ktime_sub(t.timer.expires, t.timer.base->get_time());
		if (rem.tv64 <= 0)
			return 0;
		tu = ktime_to_timespec(rem);
		if (copy_to_user(rmtp, &tu, sizeof(tu)))
			return -EFAULT;
	}

	restart = &current_thread_info()->restart_block;
	restart->fn = hrtimer_nanosleep_restart;
	restart->arg0 = (unsigned long) t.timer.base->index;
	restart->arg1 = (unsigned long) rmtp;
	restart->arg2 = t.timer.expires.tv64 & 0xFFFFFFFF;
	restart->arg3 = t.timer.expires.tv64 >> 32;

	return -ERESTART_RESTARTBLOCK;
}