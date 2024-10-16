int blkcg_activate_policy(struct request_queue *q,
			  const struct blkcg_policy *pol)
{
	struct blkg_policy_data *pd_prealloc = NULL;
	struct blkcg_gq *blkg;
	int ret;

	if (blkcg_policy_enabled(q, pol))
		return 0;

	if (q->mq_ops)
		blk_mq_freeze_queue(q);
	else
		blk_queue_bypass_start(q);
pd_prealloc:
	if (!pd_prealloc) {
		pd_prealloc = pol->pd_alloc_fn(GFP_KERNEL, q->node);
		if (!pd_prealloc) {
			ret = -ENOMEM;
			goto out_bypass_end;
		}
	}

	spin_lock_irq(q->queue_lock);

	list_for_each_entry(blkg, &q->blkg_list, q_node) {
		struct blkg_policy_data *pd;

		if (blkg->pd[pol->plid])
			continue;

		pd = pol->pd_alloc_fn(GFP_NOWAIT | __GFP_NOWARN, q->node);
		if (!pd)
			swap(pd, pd_prealloc);
		if (!pd) {
			spin_unlock_irq(q->queue_lock);
			goto pd_prealloc;
		}

		blkg->pd[pol->plid] = pd;
		pd->blkg = blkg;
		pd->plid = pol->plid;
		if (pol->pd_init_fn)
			pol->pd_init_fn(pd);
	}

	__set_bit(pol->plid, q->blkcg_pols);
	ret = 0;

	spin_unlock_irq(q->queue_lock);
out_bypass_end:
	if (q->mq_ops)
		blk_mq_unfreeze_queue(q);
	else
		blk_queue_bypass_end(q);
	if (pd_prealloc)
		pol->pd_free_fn(pd_prealloc);
	return ret;
}