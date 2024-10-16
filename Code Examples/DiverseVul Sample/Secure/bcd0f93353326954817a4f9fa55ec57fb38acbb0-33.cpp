static int pipe_do_remove(struct sock *sk)
{
	struct pep_sock *pn = pep_sk(sk);
	struct pnpipehdr *ph;
	struct sk_buff *skb;

	skb = pep_alloc_skb(sk, NULL, 0, GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	ph = pnp_hdr(skb);
	ph->utid = 0;
	ph->message_id = PNS_PIPE_REMOVE_REQ;
	ph->pipe_handle = pn->pipe_handle;
	ph->data0 = PAD;
	return pn_skb_send(sk, skb, NULL);
}