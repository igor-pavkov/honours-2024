static unsigned int selinux_ipv4_postroute(void *priv,
					   struct sk_buff *skb,
					   const struct nf_hook_state *state)
{
	return selinux_ip_postroute(skb, state->out, PF_INET);
}