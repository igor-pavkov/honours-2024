static int rose_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct rose_sock *rose;

	if (sk == NULL) return 0;

	sock_hold(sk);
	sock_orphan(sk);
	lock_sock(sk);
	rose = rose_sk(sk);

	switch (rose->state) {
	case ROSE_STATE_0:
		release_sock(sk);
		rose_disconnect(sk, 0, -1, -1);
		lock_sock(sk);
		rose_destroy_socket(sk);
		break;

	case ROSE_STATE_2:
		rose->neighbour->use--;
		release_sock(sk);
		rose_disconnect(sk, 0, -1, -1);
		lock_sock(sk);
		rose_destroy_socket(sk);
		break;

	case ROSE_STATE_1:
	case ROSE_STATE_3:
	case ROSE_STATE_4:
	case ROSE_STATE_5:
		rose_clear_queues(sk);
		rose_stop_idletimer(sk);
		rose_write_internal(sk, ROSE_CLEAR_REQUEST);
		rose_start_t3timer(sk);
		rose->state  = ROSE_STATE_2;
		sk->sk_state    = TCP_CLOSE;
		sk->sk_shutdown |= SEND_SHUTDOWN;
		sk->sk_state_change(sk);
		sock_set_flag(sk, SOCK_DEAD);
		sock_set_flag(sk, SOCK_DESTROY);
		break;

	default:
		break;
	}

	sock->sk = NULL;
	release_sock(sk);
	sock_put(sk);

	return 0;
}