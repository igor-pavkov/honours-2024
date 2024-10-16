static int xfrm_aevent_state_notify(struct xfrm_state *x, const struct km_event *c)
{
	struct net *net = xs_net(x);
	struct sk_buff *skb;
	int err;

	skb = nlmsg_new(xfrm_aevent_msgsize(x), GFP_ATOMIC);
	if (skb == NULL)
		return -ENOMEM;

	err = build_aevent(skb, x, c);
	BUG_ON(err < 0);

	return xfrm_nlmsg_multicast(net, skb, 0, XFRMNLGRP_AEVENTS);
}