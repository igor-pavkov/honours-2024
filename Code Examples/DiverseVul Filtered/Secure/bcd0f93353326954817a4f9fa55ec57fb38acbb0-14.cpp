static int pipe_handler_request(struct sock *sk, u8 id, u8 code,
				const void *data, int len)
{
	struct pep_sock *pn = pep_sk(sk);
	struct pnpipehdr *ph;
	struct sk_buff *skb;

	skb = pep_alloc_skb(sk, data, len, GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	ph = pnp_hdr(skb);
	ph->utid = id; /* whatever */
	ph->message_id = id;
	ph->pipe_handle = pn->pipe_handle;
	ph->error_code = code;
	return pn_skb_send(sk, skb, NULL);
}