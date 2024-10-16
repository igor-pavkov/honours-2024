static int ip_vs_genl_fill_stats(struct sk_buff *skb, int container_type,
				 struct ip_vs_stats *stats)
{
	struct ip_vs_stats_user ustats;
	struct nlattr *nl_stats = nla_nest_start(skb, container_type);
	if (!nl_stats)
		return -EMSGSIZE;

	ip_vs_copy_stats(&ustats, stats);

	if (nla_put_u32(skb, IPVS_STATS_ATTR_CONNS, ustats.conns) ||
	    nla_put_u32(skb, IPVS_STATS_ATTR_INPKTS, ustats.inpkts) ||
	    nla_put_u32(skb, IPVS_STATS_ATTR_OUTPKTS, ustats.outpkts) ||
	    nla_put_u64(skb, IPVS_STATS_ATTR_INBYTES, ustats.inbytes) ||
	    nla_put_u64(skb, IPVS_STATS_ATTR_OUTBYTES, ustats.outbytes) ||
	    nla_put_u32(skb, IPVS_STATS_ATTR_CPS, ustats.cps) ||
	    nla_put_u32(skb, IPVS_STATS_ATTR_INPPS, ustats.inpps) ||
	    nla_put_u32(skb, IPVS_STATS_ATTR_OUTPPS, ustats.outpps) ||
	    nla_put_u32(skb, IPVS_STATS_ATTR_INBPS, ustats.inbps) ||
	    nla_put_u32(skb, IPVS_STATS_ATTR_OUTBPS, ustats.outbps))
		goto nla_put_failure;
	nla_nest_end(skb, nl_stats);

	return 0;

nla_put_failure:
	nla_nest_cancel(skb, nl_stats);
	return -EMSGSIZE;
}