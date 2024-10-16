void blkcg_deactivate_policy(struct request_queue *q,
			     const struct blkcg_policy *pol)
{
	struct blkcg_gq *blkg;

	if (!blkcg_policy_enabled(q, pol))
		return;

	if (q->mq_ops)
		blk_mq_freeze_queue(q);
	else
		blk_queue_bypass_start(q);

	spin_lock_irq(q->queue_lock);

	__clear_bit(pol->plid, q->blkcg_pols);

	list_for_each_entry(blkg, &q->blkg_list, q_node) {
		/* grab blkcg lock too while removing @pd from @blkg */
		spin_lock(&blkg->blkcg->lock);

		if (blkg->pd[pol->plid]) {
			if (pol->pd_offline_fn)
				pol->pd_offline_fn(blkg->pd[pol->plid]);
			pol->pd_free_fn(blkg->pd[pol->plid]);
			blkg->pd[pol->plid] = NULL;
		}

		spin_unlock(&blkg->blkcg->lock);
	}

	spin_unlock_irq(q->queue_lock);

	if (q->mq_ops)
		blk_mq_unfreeze_queue(q);
	else
		blk_queue_bypass_end(q);
}