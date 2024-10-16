struct sock *nfc_llcp_accept_dequeue(struct sock *parent,
				     struct socket *newsock)
{
	struct nfc_llcp_sock *lsk, *n, *llcp_parent;
	struct sock *sk;

	llcp_parent = nfc_llcp_sock(parent);

	list_for_each_entry_safe(lsk, n, &llcp_parent->accept_queue,
				 accept_queue) {
		sk = &lsk->sk;
		lock_sock(sk);

		if (sk->sk_state == LLCP_CLOSED) {
			release_sock(sk);
			nfc_llcp_accept_unlink(sk);
			continue;
		}

		if (sk->sk_state == LLCP_CONNECTED || !newsock) {
			list_del_init(&lsk->accept_queue);
			sock_put(sk);

			if (newsock)
				sock_graft(sk, newsock);

			release_sock(sk);

			pr_debug("Returning sk state %d\n", sk->sk_state);

			sk_acceptq_removed(parent);

			return sk;
		}

		release_sock(sk);
	}

	return NULL;
}