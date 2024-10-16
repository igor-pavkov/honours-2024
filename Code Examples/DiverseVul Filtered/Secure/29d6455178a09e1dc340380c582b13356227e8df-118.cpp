static void __setscheduler(struct rq *rq, struct task_struct *p,
			   const struct sched_attr *attr, bool keep_boost)
{
	__setscheduler_params(p, attr);

	/*
	 * Keep a potential priority boosting if called from
	 * sched_setscheduler().
	 */
	if (keep_boost)
		p->prio = rt_mutex_get_effective_prio(p, normal_prio(p));
	else
		p->prio = normal_prio(p);

	if (dl_prio(p->prio))
		p->sched_class = &dl_sched_class;
	else if (rt_prio(p->prio))
		p->sched_class = &rt_sched_class;
	else
		p->sched_class = &fair_sched_class;
}