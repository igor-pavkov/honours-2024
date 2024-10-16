static void l2cap_sock_kill(struct sock *sk)
{
	if (!sock_flag(sk, SOCK_ZAPPED) || sk->sk_socket)
		return;

	BT_DBG("sk %p state %s", sk, state_to_string(sk->sk_state));

	/* Kill poor orphan */

	l2cap_chan_destroy(l2cap_pi(sk)->chan);
	sock_set_flag(sk, SOCK_DEAD);
	sock_put(sk);
}