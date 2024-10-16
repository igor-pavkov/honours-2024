static unsigned int selinux_ipv4_forward(void *priv,
					 struct sk_buff *skb,
					 const struct nf_hook_state *state)
{
	return selinux_ip_forward(skb, state->in, PF_INET);
}