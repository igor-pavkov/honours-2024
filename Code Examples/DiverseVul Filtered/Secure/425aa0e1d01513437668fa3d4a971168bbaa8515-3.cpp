static void ip_cmsg_recv_dstaddr(struct msghdr *msg, struct sk_buff *skb)
{
	__be16 _ports[2], *ports;
	struct sockaddr_in sin;

	/* All current transport protocols have the port numbers in the
	 * first four bytes of the transport header and this function is
	 * written with this assumption in mind.
	 */
	ports = skb_header_pointer(skb, skb_transport_offset(skb),
				   sizeof(_ports), &_ports);
	if (!ports)
		return;

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = ip_hdr(skb)->daddr;
	sin.sin_port = ports[1];
	memset(sin.sin_zero, 0, sizeof(sin.sin_zero));

	put_cmsg(msg, SOL_IP, IP_ORIGDSTADDR, sizeof(sin), &sin);
}