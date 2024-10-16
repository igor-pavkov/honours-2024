static int packet_sendmsg(struct socket *sock, struct msghdr *msg, size_t len)
{
	struct sock *sk = sock->sk;
	struct packet_sock *po = pkt_sk(sk);

	/* Reading tx_ring.pg_vec without holding pg_vec_lock is racy.
	 * tpacket_snd() will redo the check safely.
	 */
	if (data_race(po->tx_ring.pg_vec))
		return tpacket_snd(po, msg);

	return packet_snd(sock, msg, len);
}