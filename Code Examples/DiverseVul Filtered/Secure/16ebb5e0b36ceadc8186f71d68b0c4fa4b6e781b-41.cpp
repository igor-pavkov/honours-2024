static void notify_and_destroy(struct sk_buff *skb, struct nlmsghdr *n, u32 clid,
			       struct Qdisc *old, struct Qdisc *new)
{
	if (new || old)
		qdisc_notify(skb, n, clid, old, new);

	if (old)
		qdisc_destroy(old);
}