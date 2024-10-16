static inline __poll_t llcp_accept_poll(struct sock *parent)
{
	struct nfc_llcp_sock *llcp_sock, *parent_sock;
	struct sock *sk;

	parent_sock = nfc_llcp_sock(parent);

	list_for_each_entry(llcp_sock, &parent_sock->accept_queue,
			    accept_queue) {
		sk = &llcp_sock->sk;

		if (sk->sk_state == LLCP_CONNECTED)
			return EPOLLIN | EPOLLRDNORM;
	}

	return 0;
}