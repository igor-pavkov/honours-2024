static int ec_queue_packet(struct sock *sk, struct sk_buff *skb,
			   unsigned char stn, unsigned char net,
			   unsigned char cb, unsigned char port)
{
	struct ec_cb *eb = (struct ec_cb *)&skb->cb;
	struct sockaddr_ec *sec = (struct sockaddr_ec *)&eb->sec;

	memset(sec, 0, sizeof(struct sockaddr_ec));
	sec->sec_family = AF_ECONET;
	sec->type = ECTYPE_PACKET_RECEIVED;
	sec->port = port;
	sec->cb = cb;
	sec->addr.net = net;
	sec->addr.station = stn;

	return sock_queue_rcv_skb(sk, skb);
}