static void sco_sock_init(struct sock *sk, struct sock *parent)
{
	BT_DBG("sk %p", sk);

	if (parent)
		sk->sk_type = parent->sk_type;
}