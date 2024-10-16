static void posix_cpu_timers_init(struct task_struct *tsk)
{
	tsk->cputime_expires.prof_exp = cputime_zero;
	tsk->cputime_expires.virt_exp = cputime_zero;
	tsk->cputime_expires.sched_exp = 0;
	INIT_LIST_HEAD(&tsk->cpu_timers[0]);
	INIT_LIST_HEAD(&tsk->cpu_timers[1]);
	INIT_LIST_HEAD(&tsk->cpu_timers[2]);
}