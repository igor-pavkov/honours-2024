static void ax25_destroy_timer(struct timer_list *t)
{
	ax25_cb *ax25 = from_timer(ax25, t, dtimer);
	struct sock *sk;

	sk=ax25->sk;

	bh_lock_sock(sk);
	sock_hold(sk);
	ax25_destroy_socket(ax25);
	bh_unlock_sock(sk);
	sock_put(sk);
}