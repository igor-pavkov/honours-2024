static int sco_sock_recvmsg(struct kiocb *iocb, struct socket *sock,
			    struct msghdr *msg, size_t len, int flags)
{
	struct sock *sk = sock->sk;
	struct sco_pinfo *pi = sco_pi(sk);

	lock_sock(sk);

	if (sk->sk_state == BT_CONNECT2 &&
	    test_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags)) {
		hci_conn_accept(pi->conn->hcon, 0);
		sk->sk_state = BT_CONFIG;
		msg->msg_namelen = 0;

		release_sock(sk);
		return 0;
	}

	release_sock(sk);

	return bt_sock_recvmsg(iocb, sock, msg, len, flags);
}