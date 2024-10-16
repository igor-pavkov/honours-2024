static inline struct tun_sock *tun_sk(struct sock *sk)
{
	return container_of(sk, struct tun_sock, sk);
}