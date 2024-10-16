void nfc_llcp_accept_unlink(struct sock *sk)
{
	struct nfc_llcp_sock *llcp_sock = nfc_llcp_sock(sk);

	pr_debug("state %d\n", sk->sk_state);

	list_del_init(&llcp_sock->accept_queue);
	sk_acceptq_removed(llcp_sock->parent);
	llcp_sock->parent = NULL;

	sock_put(sk);
}