static int pep_init(struct sock *sk)
{
	struct pep_sock *pn = pep_sk(sk);

	sk->sk_destruct = pipe_destruct;
	INIT_HLIST_HEAD(&pn->hlist);
	pn->listener = NULL;
	skb_queue_head_init(&pn->ctrlreq_queue);
	atomic_set(&pn->tx_credits, 0);
	pn->ifindex = 0;
	pn->peer_type = 0;
	pn->pipe_handle = PN_PIPE_INVALID_HANDLE;
	pn->rx_credits = 0;
	pn->rx_fc = pn->tx_fc = PN_LEGACY_FLOW_CONTROL;
	pn->init_enable = 1;
	pn->aligned = 0;
	return 0;
}