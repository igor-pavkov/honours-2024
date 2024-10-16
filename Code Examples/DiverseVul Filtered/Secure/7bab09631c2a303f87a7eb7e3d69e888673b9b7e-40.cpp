static int xdst_queue_output(struct net *net, struct sock *sk, struct sk_buff *skb)
{
	unsigned long sched_next;
	struct dst_entry *dst = skb_dst(skb);
	struct xfrm_dst *xdst = (struct xfrm_dst *) dst;
	struct xfrm_policy *pol = xdst->pols[0];
	struct xfrm_policy_queue *pq = &pol->polq;

	if (unlikely(skb_fclone_busy(sk, skb))) {
		kfree_skb(skb);
		return 0;
	}

	if (pq->hold_queue.qlen > XFRM_MAX_QUEUE_LEN) {
		kfree_skb(skb);
		return -EAGAIN;
	}

	skb_dst_force(skb);

	spin_lock_bh(&pq->hold_queue.lock);

	if (!pq->timeout)
		pq->timeout = XFRM_QUEUE_TMO_MIN;

	sched_next = jiffies + pq->timeout;

	if (del_timer(&pq->hold_timer)) {
		if (time_before(pq->hold_timer.expires, sched_next))
			sched_next = pq->hold_timer.expires;
		xfrm_pol_put(pol);
	}

	__skb_queue_tail(&pq->hold_queue, skb);
	if (!mod_timer(&pq->hold_timer, sched_next))
		xfrm_pol_hold(pol);

	spin_unlock_bh(&pq->hold_queue.lock);

	return 0;
}