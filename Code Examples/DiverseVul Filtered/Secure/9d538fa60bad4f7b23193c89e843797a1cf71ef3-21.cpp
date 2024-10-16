int sk_set_peek_off(struct sock *sk, int val)
{
	sk->sk_peek_off = val;
	return 0;
}