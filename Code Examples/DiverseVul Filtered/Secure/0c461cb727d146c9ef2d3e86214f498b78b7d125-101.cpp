static unsigned int selinux_ipv4_output(void *priv,
					struct sk_buff *skb,
					const struct nf_hook_state *state)
{
	return selinux_ip_output(skb, PF_INET);
}