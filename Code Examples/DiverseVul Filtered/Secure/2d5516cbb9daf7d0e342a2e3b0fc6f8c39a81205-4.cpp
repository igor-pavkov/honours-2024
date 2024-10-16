static int copy_signal(unsigned long clone_flags, struct task_struct *tsk)
{
	struct signal_struct *sig;

	if (clone_flags & CLONE_THREAD) {
		atomic_inc(&current->signal->count);
		atomic_inc(&current->signal->live);
		return 0;
	}
	sig = kmem_cache_alloc(signal_cachep, GFP_KERNEL);

	if (sig)
		posix_cpu_timers_init_group(sig);

	tsk->signal = sig;
	if (!sig)
		return -ENOMEM;

	atomic_set(&sig->count, 1);
	atomic_set(&sig->live, 1);
	init_waitqueue_head(&sig->wait_chldexit);
	sig->flags = 0;
	sig->group_exit_code = 0;
	sig->group_exit_task = NULL;
	sig->group_stop_count = 0;
	sig->curr_target = tsk;
	init_sigpending(&sig->shared_pending);
	INIT_LIST_HEAD(&sig->posix_timers);

	hrtimer_init(&sig->real_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	sig->it_real_incr.tv64 = 0;
	sig->real_timer.function = it_real_fn;

	sig->leader = 0;	/* session leadership doesn't inherit */
	sig->tty_old_pgrp = NULL;
	sig->tty = NULL;

	sig->utime = sig->stime = sig->cutime = sig->cstime = cputime_zero;
	sig->gtime = cputime_zero;
	sig->cgtime = cputime_zero;
	sig->nvcsw = sig->nivcsw = sig->cnvcsw = sig->cnivcsw = 0;
	sig->min_flt = sig->maj_flt = sig->cmin_flt = sig->cmaj_flt = 0;
	sig->inblock = sig->oublock = sig->cinblock = sig->coublock = 0;
	task_io_accounting_init(&sig->ioac);
	sig->sum_sched_runtime = 0;
	taskstats_tgid_init(sig);

	task_lock(current->group_leader);
	memcpy(sig->rlim, current->signal->rlim, sizeof sig->rlim);
	task_unlock(current->group_leader);

	acct_init_pacct(&sig->pacct);

	tty_audit_fork(sig);

	return 0;
}