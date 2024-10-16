static int xfrm_send_migrate(const struct xfrm_selector *sel, u8 dir, u8 type,
			     const struct xfrm_migrate *m, int num_migrate,
			     const struct xfrm_kmaddress *k,
			     const struct xfrm_encap_tmpl *encap)
{
	struct net *net = &init_net;
	struct sk_buff *skb;
	int err;

	skb = nlmsg_new(xfrm_migrate_msgsize(num_migrate, !!k, !!encap),
			GFP_ATOMIC);
	if (skb == NULL)
		return -ENOMEM;

	/* build migrate */
	err = build_migrate(skb, m, num_migrate, k, sel, encap, dir, type);
	BUG_ON(err < 0);

	return xfrm_nlmsg_multicast(net, skb, 0, XFRMNLGRP_MIGRATE);
}