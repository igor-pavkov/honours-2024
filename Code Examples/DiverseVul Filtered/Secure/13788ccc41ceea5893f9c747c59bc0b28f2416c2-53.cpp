long __sched hrtimer_nanosleep_restart(struct restart_block *restart)
{
	struct hrtimer_sleeper t;
	struct timespec __user *rmtp;
	struct timespec tu;
	ktime_t time;

	restart->fn = do_no_restart_syscall;

	hrtimer_init(&t.timer, restart->arg0, HRTIMER_MODE_ABS);
	t.timer.expires.tv64 = ((u64)restart->arg3 << 32) | (u64) restart->arg2;

	if (do_nanosleep(&t, HRTIMER_MODE_ABS))
		return 0;

	rmtp = (struct timespec __user *) restart->arg1;
	if (rmtp) {
		time = ktime_sub(t.timer.expires, t.timer.base->get_time());
		if (time.tv64 <= 0)
			return 0;
		tu = ktime_to_timespec(time);
		if (copy_to_user(rmtp, &tu, sizeof(tu)))
			return -EFAULT;
	}

	restart->fn = hrtimer_nanosleep_restart;

	/* The other values in restart are already filled in */
	return -ERESTART_RESTARTBLOCK;
}