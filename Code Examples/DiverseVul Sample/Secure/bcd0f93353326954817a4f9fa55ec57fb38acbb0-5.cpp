static int pipe_skb_send(struct sock *sk, struct sk_buff *skb)
{
	struct pep_sock *pn = pep_sk(sk);
	struct pnpipehdr *ph;
	int err;

	if (pn_flow_safe(pn->tx_fc) &&
	    !atomic_add_unless(&pn->tx_credits, -1, 0)) {
		kfree_skb(skb);
		return -ENOBUFS;
	}

	skb_push(skb, 3 + pn->aligned);
	skb_reset_transport_header(skb);
	ph = pnp_hdr(skb);
	ph->utid = 0;
	if (pn->aligned) {
		ph->message_id = PNS_PIPE_ALIGNED_DATA;
		ph->data0 = 0; /* padding */
	} else
		ph->message_id = PNS_PIPE_DATA;
	ph->pipe_handle = pn->pipe_handle;
	err = pn_skb_send(sk, skb, NULL);

	if (err && pn_flow_safe(pn->tx_fc))
		atomic_inc(&pn->tx_credits);
	return err;

}