static int llcp_sock_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;
	int ret = 0;

	pr_debug("sk %p backlog %d\n", sk, backlog);

	lock_sock(sk);

	if ((sock->type != SOCK_SEQPACKET && sock->type != SOCK_STREAM) ||
	    sk->sk_state != LLCP_BOUND) {
		ret = -EBADFD;
		goto error;
	}

	sk->sk_max_ack_backlog = backlog;
	sk->sk_ack_backlog = 0;

	pr_debug("Socket listening\n");
	sk->sk_state = LLCP_LISTEN;

error:
	release_sock(sk);

	return ret;
}