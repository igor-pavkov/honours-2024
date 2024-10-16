unsigned long long task_delta_exec(struct task_struct *p)
{
	unsigned long flags;
	struct rq *rq;
	u64 ns = 0;

	rq = task_rq_lock(p, &flags);
	ns = do_task_delta_exec(p, rq);
	task_rq_unlock(rq, p, &flags);

	return ns;
}