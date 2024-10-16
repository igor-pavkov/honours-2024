static int llc_ui_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;
	int rc = -EINVAL;

	lock_sock(sk);
	if (unlikely(sock->state != SS_UNCONNECTED))
		goto out;
	rc = -EOPNOTSUPP;
	if (unlikely(sk->sk_type != SOCK_STREAM))
		goto out;
	rc = -EAGAIN;
	if (sock_flag(sk, SOCK_ZAPPED))
		goto out;
	rc = 0;
	if (!(unsigned)backlog)	/* BSDism */
		backlog = 1;
	sk->sk_max_ack_backlog = backlog;
	if (sk->sk_state != TCP_LISTEN) {
		sk->sk_ack_backlog = 0;
		sk->sk_state	   = TCP_LISTEN;
	}
	sk->sk_socket->flags |= __SO_ACCEPTCON;
out:
	release_sock(sk);
	return rc;
}