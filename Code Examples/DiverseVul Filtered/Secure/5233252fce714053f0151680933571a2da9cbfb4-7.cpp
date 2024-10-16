static void sco_sock_set_timer(struct sock *sk, long timeout)
{
	BT_DBG("sock %p state %d timeout %ld", sk, sk->sk_state, timeout);
	sk_reset_timer(sk, &sk->sk_timer, jiffies + timeout);
}