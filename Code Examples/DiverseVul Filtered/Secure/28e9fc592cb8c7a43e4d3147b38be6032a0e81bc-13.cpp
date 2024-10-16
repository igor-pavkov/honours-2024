static void llc_ui_sk_init(struct socket *sock, struct sock *sk)
{
	sock_graft(sk, sock);
	sk->sk_type	= sock->type;
	sock->ops	= &llc_ui_ops;
}