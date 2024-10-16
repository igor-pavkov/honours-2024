static int pep_do_rcv(struct sock *sk, struct sk_buff *skb)
{
	struct pep_sock *pn = pep_sk(sk);
	struct sock *sknode;
	struct pnpipehdr *hdr;
	struct sockaddr_pn dst;
	u8 pipe_handle;

	if (!pskb_may_pull(skb, sizeof(*hdr)))
		goto drop;

	hdr = pnp_hdr(skb);
	pipe_handle = hdr->pipe_handle;
	if (pipe_handle == PN_PIPE_INVALID_HANDLE)
		goto drop;

	pn_skb_get_dst_sockaddr(skb, &dst);

	/* Look for an existing pipe handle */
	sknode = pep_find_pipe(&pn->hlist, &dst, pipe_handle);
	if (sknode)
		return sk_receive_skb(sknode, skb, 1);

	switch (hdr->message_id) {
	case PNS_PEP_CONNECT_REQ:
		if (sk->sk_state != TCP_LISTEN || sk_acceptq_is_full(sk)) {
			pep_reject_conn(sk, skb, PN_PIPE_ERR_PEP_IN_USE,
					GFP_ATOMIC);
			break;
		}
		skb_queue_head(&sk->sk_receive_queue, skb);
		sk_acceptq_added(sk);
		if (!sock_flag(sk, SOCK_DEAD))
			sk->sk_data_ready(sk);
		return NET_RX_SUCCESS;

	case PNS_PEP_DISCONNECT_REQ:
		pep_reply(sk, skb, PN_PIPE_NO_ERROR, NULL, 0, GFP_ATOMIC);
		break;

	case PNS_PEP_CTRL_REQ:
		pep_ctrlreq_error(sk, skb, PN_PIPE_INVALID_HANDLE, GFP_ATOMIC);
		break;

	case PNS_PEP_RESET_REQ:
	case PNS_PEP_ENABLE_REQ:
	case PNS_PEP_DISABLE_REQ:
		/* invalid handle is not even allowed here! */
		break;

	default:
		if ((1 << sk->sk_state)
				& ~(TCPF_CLOSE|TCPF_LISTEN|TCPF_CLOSE_WAIT))
			/* actively connected socket */
			return pipe_handler_do_rcv(sk, skb);
	}
drop:
	kfree_skb(skb);
	return NET_RX_SUCCESS;
}