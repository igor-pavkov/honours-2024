static int xfrm_exp_policy_notify(struct xfrm_policy *xp, int dir, const struct km_event *c)
{
	struct net *net = xp_net(xp);
	struct sk_buff *skb;
	int err;

	skb = nlmsg_new(xfrm_polexpire_msgsize(xp), GFP_ATOMIC);
	if (skb == NULL)
		return -ENOMEM;

	err = build_polexpire(skb, xp, dir, c);
	BUG_ON(err < 0);

	return xfrm_nlmsg_multicast(net, skb, 0, XFRMNLGRP_EXPIRE);
}