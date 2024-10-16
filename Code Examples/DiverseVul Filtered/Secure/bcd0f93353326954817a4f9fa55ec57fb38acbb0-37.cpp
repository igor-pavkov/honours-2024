static int pep_indicate(struct sock *sk, u8 id, u8 code,
			const void *data, int len, gfp_t priority)
{
	struct pep_sock *pn = pep_sk(sk);
	struct pnpipehdr *ph;
	struct sk_buff *skb;

	skb = pep_alloc_skb(sk, data, len, priority);
	if (!skb)
		return -ENOMEM;

	ph = pnp_hdr(skb);
	ph->utid = 0;
	ph->message_id = id;
	ph->pipe_handle = pn->pipe_handle;
	ph->error_code = code;
	return pn_skb_send(sk, skb, NULL);
}