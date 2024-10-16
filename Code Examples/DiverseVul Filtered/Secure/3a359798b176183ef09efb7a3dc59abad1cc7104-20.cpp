void nfc_llcp_accept_enqueue(struct sock *parent, struct sock *sk)
{
	struct nfc_llcp_sock *llcp_sock = nfc_llcp_sock(sk);
	struct nfc_llcp_sock *llcp_sock_parent = nfc_llcp_sock(parent);

	/* Lock will be free from unlink */
	sock_hold(sk);

	list_add_tail(&llcp_sock->accept_queue,
		      &llcp_sock_parent->accept_queue);
	llcp_sock->parent = parent;
	sk_acceptq_added(parent);
}