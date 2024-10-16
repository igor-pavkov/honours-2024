static void __rfcomm_sock_close(struct sock *sk)
{
	struct rfcomm_dlc *d = rfcomm_pi(sk)->dlc;

	BT_DBG("sk %p state %d socket %p", sk, sk->sk_state, sk->sk_socket);

	switch (sk->sk_state) {
	case BT_LISTEN:
		rfcomm_sock_cleanup_listen(sk);
		break;

	case BT_CONNECT:
	case BT_CONNECT2:
	case BT_CONFIG:
	case BT_CONNECTED:
		rfcomm_dlc_close(d, 0);

	default:
		sock_set_flag(sk, SOCK_ZAPPED);
		break;
	}
}