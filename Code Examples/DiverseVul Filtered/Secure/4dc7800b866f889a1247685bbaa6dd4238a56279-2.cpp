int fr_dhcp_send(RADIUS_PACKET *packet)
{
	struct sockaddr_storage	dst;
	socklen_t		sizeof_dst;
#ifdef WITH_UDPFROMTO
	struct sockaddr_storage	src;
	socklen_t		sizeof_src;
#endif

	fr_ipaddr2sockaddr(&packet->dst_ipaddr, packet->dst_port,
			   &dst, &sizeof_dst);

#ifndef WITH_UDPFROMTO
	/*
	 *	Assume that the packet is encoded before sending it.
	 */
	return sendto(packet->sockfd, packet->data, packet->data_len, 0,
		      (struct sockaddr *)&dst, sizeof_dst);
#else
	fr_ipaddr2sockaddr(&packet->src_ipaddr, packet->src_port,
			   &src, &sizeof_src);

	return sendfromto(packet->sockfd,
			  packet->data, packet->data_len, 0,
			  (struct sockaddr *)&src, sizeof_src,
			  (struct sockaddr *)&dst, sizeof_dst);
#endif
}