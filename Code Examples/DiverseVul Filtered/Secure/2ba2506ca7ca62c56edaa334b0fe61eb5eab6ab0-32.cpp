static inline int dev_requeue_skb(struct sk_buff *skb, struct net_device *dev,
				  struct Qdisc *q)
{
	if (unlikely(skb->next))
		dev->gso_skb = skb;
	else
		q->ops->requeue(skb, q);

	netif_schedule(dev);
	return 0;
}