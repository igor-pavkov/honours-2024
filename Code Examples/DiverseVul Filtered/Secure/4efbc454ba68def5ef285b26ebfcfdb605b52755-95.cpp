static inline void pre_schedule(struct rq *rq, struct task_struct *prev)
{
	if (prev->sched_class->pre_schedule)
		prev->sched_class->pre_schedule(rq, prev);
}