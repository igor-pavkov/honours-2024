static void nr_destroy_timer(unsigned long data)
{
	struct sock *sk=(struct sock *)data;
	bh_lock_sock(sk);
	sock_hold(sk);
	nr_destroy_socket(sk);
	bh_unlock_sock(sk);
	sock_put(sk);
}