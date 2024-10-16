static int dgram_init(struct sock *sk)
{
	struct dgram_sock *ro = dgram_sk(sk);

	ro->want_ack = 1;
	ro->want_lqi = 0;
	return 0;
}