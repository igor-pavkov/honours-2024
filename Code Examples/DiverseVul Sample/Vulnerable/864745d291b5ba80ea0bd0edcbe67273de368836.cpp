static struct sk_buff *xfrm_state_netlink(struct sk_buff *in_skb,
					  struct xfrm_state *x, u32 seq)
{
	struct xfrm_dump_info info;
	struct sk_buff *skb;

	skb = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_ATOMIC);
	if (!skb)
		return ERR_PTR(-ENOMEM);

	info.in_skb = in_skb;
	info.out_skb = skb;
	info.nlmsg_seq = seq;
	info.nlmsg_flags = 0;

	if (dump_one_state(x, 0, &info)) {
		kfree_skb(skb);
		return NULL;
	}

	return skb;
}