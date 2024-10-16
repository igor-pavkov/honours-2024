static int ip6gre_fill_info(struct sk_buff *skb, const struct net_device *dev)
{
	struct ip6_tnl *t = netdev_priv(dev);
	struct __ip6_tnl_parm *p = &t->parms;

	if (nla_put_u32(skb, IFLA_GRE_LINK, p->link) ||
	    nla_put_be16(skb, IFLA_GRE_IFLAGS,
			 gre_tnl_flags_to_gre_flags(p->i_flags)) ||
	    nla_put_be16(skb, IFLA_GRE_OFLAGS,
			 gre_tnl_flags_to_gre_flags(p->o_flags)) ||
	    nla_put_be32(skb, IFLA_GRE_IKEY, p->i_key) ||
	    nla_put_be32(skb, IFLA_GRE_OKEY, p->o_key) ||
	    nla_put_in6_addr(skb, IFLA_GRE_LOCAL, &p->laddr) ||
	    nla_put_in6_addr(skb, IFLA_GRE_REMOTE, &p->raddr) ||
	    nla_put_u8(skb, IFLA_GRE_TTL, p->hop_limit) ||
	    nla_put_u8(skb, IFLA_GRE_ENCAP_LIMIT, p->encap_limit) ||
	    nla_put_be32(skb, IFLA_GRE_FLOWINFO, p->flowinfo) ||
	    nla_put_u32(skb, IFLA_GRE_FLAGS, p->flags))
		goto nla_put_failure;

	if (nla_put_u16(skb, IFLA_GRE_ENCAP_TYPE,
			t->encap.type) ||
	    nla_put_be16(skb, IFLA_GRE_ENCAP_SPORT,
			 t->encap.sport) ||
	    nla_put_be16(skb, IFLA_GRE_ENCAP_DPORT,
			 t->encap.dport) ||
	    nla_put_u16(skb, IFLA_GRE_ENCAP_FLAGS,
			t->encap.flags))
		goto nla_put_failure;

	return 0;

nla_put_failure:
	return -EMSGSIZE;
}