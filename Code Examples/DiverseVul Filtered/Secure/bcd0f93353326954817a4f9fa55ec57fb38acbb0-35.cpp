static int pep_reply(struct sock *sk, struct sk_buff *oskb, u8 code,
			const void *data, int len, gfp_t priority)
{
	const struct pnpipehdr *oph = pnp_hdr(oskb);
	struct pnpipehdr *ph;
	struct sk_buff *skb;
	struct sockaddr_pn peer;

	skb = pep_alloc_skb(sk, data, len, priority);
	if (!skb)
		return -ENOMEM;

	ph = pnp_hdr(skb);
	ph->utid = oph->utid;
	ph->message_id = oph->message_id + 1; /* REQ -> RESP */
	ph->pipe_handle = oph->pipe_handle;
	ph->error_code = code;

	pn_skb_get_src_sockaddr(oskb, &peer);
	return pn_skb_send(sk, skb, &peer);
}