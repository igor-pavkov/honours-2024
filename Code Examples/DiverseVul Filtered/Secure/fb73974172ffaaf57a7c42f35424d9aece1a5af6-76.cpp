static int selinux_socket_socketpair(struct socket *socka,
				     struct socket *sockb)
{
	struct sk_security_struct *sksec_a = socka->sk->sk_security;
	struct sk_security_struct *sksec_b = sockb->sk->sk_security;

	sksec_a->peer_sid = sksec_b->sid;
	sksec_b->peer_sid = sksec_a->sid;

	return 0;
}