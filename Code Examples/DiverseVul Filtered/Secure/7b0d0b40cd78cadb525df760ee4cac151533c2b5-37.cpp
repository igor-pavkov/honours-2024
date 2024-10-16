static unsigned int selinux_ipv6_postroute(const struct nf_hook_ops *ops,
					   struct sk_buff *skb,
					   const struct net_device *in,
					   const struct net_device *out,
					   int (*okfn)(struct sk_buff *))
{
	return selinux_ip_postroute(skb, out->ifindex, PF_INET6);
}