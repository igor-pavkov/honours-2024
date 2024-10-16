void sched_ttwu_pending(void)
{
	struct rq *rq = this_rq();
	struct llist_node *llist = llist_del_all(&rq->wake_list);
	struct task_struct *p;
	unsigned long flags;

	if (!llist)
		return;

	raw_spin_lock_irqsave(&rq->lock, flags);
	lockdep_pin_lock(&rq->lock);

	while (llist) {
		p = llist_entry(llist, struct task_struct, wake_entry);
		llist = llist_next(llist);
		ttwu_do_activate(rq, p, 0);
	}

	lockdep_unpin_lock(&rq->lock);
	raw_spin_unlock_irqrestore(&rq->lock, flags);
}