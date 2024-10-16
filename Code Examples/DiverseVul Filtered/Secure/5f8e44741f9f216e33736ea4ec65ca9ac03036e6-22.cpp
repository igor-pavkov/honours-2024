static noinline_for_stack int rtnl_fill_stats(struct sk_buff *skb,
					      struct net_device *dev)
{
	const struct rtnl_link_stats64 *stats;
	struct rtnl_link_stats64 temp;
	struct nlattr *attr;

	stats = dev_get_stats(dev, &temp);

	attr = nla_reserve(skb, IFLA_STATS,
			   sizeof(struct rtnl_link_stats));
	if (!attr)
		return -EMSGSIZE;

	copy_rtnl_link_stats(nla_data(attr), stats);

	attr = nla_reserve(skb, IFLA_STATS64,
			   sizeof(struct rtnl_link_stats64));
	if (!attr)
		return -EMSGSIZE;

	copy_rtnl_link_stats64(nla_data(attr), stats);

	return 0;
}