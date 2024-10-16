static int ing_filter(struct sk_buff *skb, struct netdev_queue *rxq)
{
	struct net_device *dev = skb->dev;
	u32 ttl = G_TC_RTTL(skb->tc_verd);
	int result = TC_ACT_OK;
	struct Qdisc *q;

	if (unlikely(MAX_RED_LOOP < ttl++)) {
		if (net_ratelimit())
			pr_warning( "Redir loop detected Dropping packet (%d->%d)\n",
			       skb->skb_iif, dev->ifindex);
		return TC_ACT_SHOT;
	}

	skb->tc_verd = SET_TC_RTTL(skb->tc_verd, ttl);
	skb->tc_verd = SET_TC_AT(skb->tc_verd, AT_INGRESS);

	q = rxq->qdisc;
	if (q != &noop_qdisc) {
		spin_lock(qdisc_lock(q));
		if (likely(!test_bit(__QDISC_STATE_DEACTIVATED, &q->state)))
			result = qdisc_enqueue_root(skb, q);
		spin_unlock(qdisc_lock(q));
	}

	return result;
}