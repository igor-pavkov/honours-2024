static unsigned int selinux_ipv6_output(void *priv,
					struct sk_buff *skb,
					const struct nf_hook_state *state)
{
	return selinux_ip_output(skb, PF_INET6);
}