static int sco_sock_shutdown(struct socket *sock, int how)
{
	struct sock *sk = sock->sk;
	int err = 0;

	BT_DBG("sock %p, sk %p", sock, sk);

	if (!sk)
		return 0;

	sock_hold(sk);
	lock_sock(sk);

	if (!sk->sk_shutdown) {
		sk->sk_shutdown = SHUTDOWN_MASK;
		sco_sock_clear_timer(sk);
		__sco_sock_close(sk);

		if (sock_flag(sk, SOCK_LINGER) && sk->sk_lingertime &&
		    !(current->flags & PF_EXITING))
			err = bt_sock_wait_state(sk, BT_CLOSED,
						 sk->sk_lingertime);
	}

	release_sock(sk);
	sock_put(sk);

	return err;
}