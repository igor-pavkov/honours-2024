static int econet_bind(struct socket *sock, struct sockaddr *uaddr, int addr_len)
{
	struct sockaddr_ec *sec = (struct sockaddr_ec *)uaddr;
	struct sock *sk;
	struct econet_sock *eo;

	/*
	 *	Check legality
	 */

	if (addr_len < sizeof(struct sockaddr_ec) ||
	    sec->sec_family != AF_ECONET)
		return -EINVAL;

	mutex_lock(&econet_mutex);

	sk = sock->sk;
	eo = ec_sk(sk);

	eo->cb	    = sec->cb;
	eo->port    = sec->port;
	eo->station = sec->addr.station;
	eo->net	    = sec->addr.net;

	mutex_unlock(&econet_mutex);

	return 0;
}