static int rtnl_net_dumpid(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	struct rtnl_net_dump_cb net_cb = {
		.net = net,
		.skb = skb,
		.cb = cb,
		.idx = 0,
		.s_idx = cb->args[0],
	};

	spin_lock_bh(&net->nsid_lock);
	idr_for_each(&net->netns_ids, rtnl_net_dumpid_one, &net_cb);
	spin_unlock_bh(&net->nsid_lock);

	cb->args[0] = net_cb.idx;
	return skb->len;
}