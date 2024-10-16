static int ttwu_remote(struct task_struct *p, int wake_flags)
{
	struct rq *rq;
	int ret = 0;

	rq = __task_rq_lock(p);
	if (task_on_rq_queued(p)) {
		/* check_preempt_curr() may use rq clock */
		update_rq_clock(rq);
		ttwu_do_wakeup(rq, p, wake_flags);
		ret = 1;
	}
	__task_rq_unlock(rq);

	return ret;
}