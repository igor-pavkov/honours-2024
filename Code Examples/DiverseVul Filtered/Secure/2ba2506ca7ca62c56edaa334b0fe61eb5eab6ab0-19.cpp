static inline struct sk_buff *dev_dequeue_skb(struct net_device *dev,
					      struct Qdisc *q)
{
	struct sk_buff *skb;

	if ((skb = dev->gso_skb))
		dev->gso_skb = NULL;
	else
		skb = q->dequeue(q);

	return skb;
}