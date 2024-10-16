static int packet_getname(struct socket *sock, struct sockaddr *uaddr,
			  int peer)
{
	struct net_device *dev;
	struct sock *sk = sock->sk;
	struct packet_sock *po = pkt_sk(sk);
	DECLARE_SOCKADDR(struct sockaddr_ll *, sll, uaddr);
	int ifindex;

	if (peer)
		return -EOPNOTSUPP;

	ifindex = READ_ONCE(po->ifindex);
	sll->sll_family = AF_PACKET;
	sll->sll_ifindex = ifindex;
	sll->sll_protocol = READ_ONCE(po->num);
	sll->sll_pkttype = 0;
	rcu_read_lock();
	dev = dev_get_by_index_rcu(sock_net(sk), ifindex);
	if (dev) {
		sll->sll_hatype = dev->type;
		sll->sll_halen = dev->addr_len;
		memcpy(sll->sll_addr, dev->dev_addr, dev->addr_len);
	} else {
		sll->sll_hatype = 0;	/* Bad: we have no ARPHRD_UNSPEC */
		sll->sll_halen = 0;
	}
	rcu_read_unlock();

	return offsetof(struct sockaddr_ll, sll_addr) + sll->sll_halen;
}