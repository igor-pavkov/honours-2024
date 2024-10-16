void sched_offline_group(struct task_group *tg)
{
	unsigned long flags;

	/* end participation in shares distribution */
	unregister_fair_sched_group(tg);

	spin_lock_irqsave(&task_group_lock, flags);
	list_del_rcu(&tg->list);
	list_del_rcu(&tg->siblings);
	spin_unlock_irqrestore(&task_group_lock, flags);
}