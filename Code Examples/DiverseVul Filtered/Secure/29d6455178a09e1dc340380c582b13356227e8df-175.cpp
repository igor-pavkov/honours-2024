static void calc_load_migrate(struct rq *rq)
{
	long delta = calc_load_fold_active(rq);
	if (delta)
		atomic_long_add(delta, &calc_load_tasks);
}