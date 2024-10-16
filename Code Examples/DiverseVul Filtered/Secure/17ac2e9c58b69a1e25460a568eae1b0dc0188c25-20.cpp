static int rose_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;

	if (sk->sk_state != TCP_LISTEN) {
		struct rose_sock *rose = rose_sk(sk);

		rose->dest_ndigis = 0;
		memset(&rose->dest_addr, 0, ROSE_ADDR_LEN);
		memset(&rose->dest_call, 0, AX25_ADDR_LEN);
		memset(rose->dest_digis, 0, AX25_ADDR_LEN * ROSE_MAX_DIGIS);
		sk->sk_max_ack_backlog = backlog;
		sk->sk_state           = TCP_LISTEN;
		return 0;
	}

	return -EOPNOTSUPP;
}