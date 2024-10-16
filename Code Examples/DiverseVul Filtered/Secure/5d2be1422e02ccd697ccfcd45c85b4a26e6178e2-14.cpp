static int tipc_skb_tailroom(struct sk_buff *skb)
{
	int tailroom;
	int limit;

	tailroom = skb_tailroom(skb);
	limit = TIPC_SKB_MAX - skb->len;

	if (tailroom < limit)
		return tailroom;

	return limit;
}