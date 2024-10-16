static int xfrm_send_mapping(struct xfrm_state *x, xfrm_address_t *ipaddr,
			     __be16 sport)
{
	struct net *net = xs_net(x);
	struct sk_buff *skb;
	int err;

	if (x->id.proto != IPPROTO_ESP)
		return -EINVAL;

	if (!x->encap)
		return -EINVAL;

	skb = nlmsg_new(xfrm_mapping_msgsize(), GFP_ATOMIC);
	if (skb == NULL)
		return -ENOMEM;

	err = build_mapping(skb, x, ipaddr, sport);
	BUG_ON(err < 0);

	return xfrm_nlmsg_multicast(net, skb, 0, XFRMNLGRP_MAPPING);
}