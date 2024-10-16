static int nr_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct nr_sock *nr;

	if (sk == NULL) return 0;

	sock_hold(sk);
	sock_orphan(sk);
	lock_sock(sk);
	nr = nr_sk(sk);

	switch (nr->state) {
	case NR_STATE_0:
	case NR_STATE_1:
	case NR_STATE_2:
		nr_disconnect(sk, 0);
		nr_destroy_socket(sk);
		break;

	case NR_STATE_3:
		nr_clear_queues(sk);
		nr->n2count = 0;
		nr_write_internal(sk, NR_DISCREQ);
		nr_start_t1timer(sk);
		nr_stop_t2timer(sk);
		nr_stop_t4timer(sk);
		nr_stop_idletimer(sk);
		nr->state    = NR_STATE_2;
		sk->sk_state    = TCP_CLOSE;
		sk->sk_shutdown |= SEND_SHUTDOWN;
		sk->sk_state_change(sk);
		sock_set_flag(sk, SOCK_DESTROY);
		break;

	default:
		break;
	}

	sock->sk   = NULL;
	release_sock(sk);
	sock_put(sk);

	return 0;
}