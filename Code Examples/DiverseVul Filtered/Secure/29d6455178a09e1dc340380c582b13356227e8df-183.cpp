static bool dl_param_changed(struct task_struct *p,
		const struct sched_attr *attr)
{
	struct sched_dl_entity *dl_se = &p->dl;

	if (dl_se->dl_runtime != attr->sched_runtime ||
		dl_se->dl_deadline != attr->sched_deadline ||
		dl_se->dl_period != attr->sched_period ||
		dl_se->flags != attr->sched_flags)
		return true;

	return false;
}