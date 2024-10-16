struct psi_trigger *psi_trigger_create(struct psi_group *group,
			char *buf, size_t nbytes, enum psi_res res)
{
	struct psi_trigger *t;
	enum psi_states state;
	u32 threshold_us;
	u32 window_us;

	if (static_branch_likely(&psi_disabled))
		return ERR_PTR(-EOPNOTSUPP);

	if (sscanf(buf, "some %u %u", &threshold_us, &window_us) == 2)
		state = PSI_IO_SOME + res * 2;
	else if (sscanf(buf, "full %u %u", &threshold_us, &window_us) == 2)
		state = PSI_IO_FULL + res * 2;
	else
		return ERR_PTR(-EINVAL);

	if (state >= PSI_NONIDLE)
		return ERR_PTR(-EINVAL);

	if (window_us < WINDOW_MIN_US ||
		window_us > WINDOW_MAX_US)
		return ERR_PTR(-EINVAL);

	/* Check threshold */
	if (threshold_us == 0 || threshold_us > window_us)
		return ERR_PTR(-EINVAL);

	t = kmalloc(sizeof(*t), GFP_KERNEL);
	if (!t)
		return ERR_PTR(-ENOMEM);

	t->group = group;
	t->state = state;
	t->threshold = threshold_us * NSEC_PER_USEC;
	t->win.size = window_us * NSEC_PER_USEC;
	window_reset(&t->win, 0, 0, 0);

	t->event = 0;
	t->last_event_time = 0;
	init_waitqueue_head(&t->event_wait);
	kref_init(&t->refcount);

	mutex_lock(&group->trigger_lock);

	if (!rcu_access_pointer(group->poll_kworker)) {
		struct sched_param param = {
			.sched_priority = 1,
		};
		struct kthread_worker *kworker;

		kworker = kthread_create_worker(0, "psimon");
		if (IS_ERR(kworker)) {
			kfree(t);
			mutex_unlock(&group->trigger_lock);
			return ERR_CAST(kworker);
		}
		sched_setscheduler_nocheck(kworker->task, SCHED_FIFO, &param);
		kthread_init_delayed_work(&group->poll_work,
				psi_poll_work);
		rcu_assign_pointer(group->poll_kworker, kworker);
	}

	list_add(&t->node, &group->triggers);
	group->poll_min_period = min(group->poll_min_period,
		div_u64(t->win.size, UPDATES_PER_WINDOW));
	group->nr_triggers[t->state]++;
	group->poll_states |= (1 << t->state);

	mutex_unlock(&group->trigger_lock);

	return t;
}