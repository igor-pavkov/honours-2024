static void xfrm_policy_requeue(struct xfrm_policy *old,
				struct xfrm_policy *new)
{
	struct xfrm_policy_queue *pq = &old->polq;
	struct sk_buff_head list;

	if (skb_queue_empty(&pq->hold_queue))
		return;

	__skb_queue_head_init(&list);

	spin_lock_bh(&pq->hold_queue.lock);
	skb_queue_splice_init(&pq->hold_queue, &list);
	if (del_timer(&pq->hold_timer))
		xfrm_pol_put(old);
	spin_unlock_bh(&pq->hold_queue.lock);

	pq = &new->polq;

	spin_lock_bh(&pq->hold_queue.lock);
	skb_queue_splice(&list, &pq->hold_queue);
	pq->timeout = XFRM_QUEUE_TMO_MIN;
	if (!mod_timer(&pq->hold_timer, jiffies))
		xfrm_pol_hold(new);
	spin_unlock_bh(&pq->hold_queue.lock);
}