static int udhcp_raw_socket(int ifindex)
{
	int fd;
	struct sockaddr_ll sock;

	log1("Opening raw socket on ifindex %d", ifindex); //log2?

	fd = xsocket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));
	/* ^^^^^
	 * SOCK_DGRAM: remove link-layer headers on input (SOCK_RAW keeps them)
	 * ETH_P_IP: want to receive only packets with IPv4 eth type
	 */
	log1("Got raw socket fd"); //log2?

	sock.sll_family = AF_PACKET;
	sock.sll_protocol = htons(ETH_P_IP);
	sock.sll_ifindex = ifindex;
	xbind(fd, (struct sockaddr *) &sock, sizeof(sock));

#if 0 /* Several users reported breakage when BPF filter is used */
	if (CLIENT_PORT == 68) {
		/* Use only if standard port is in use */
		/*
		 *	I've selected not to see LL header, so BPF doesn't see it, too.
		 *	The filter may also pass non-IP and non-ARP packets, but we do
		 *	a more complete check when receiving the message in userspace.
		 *
		 * and filter shamelessly stolen from:
		 *
		 *	http://www.flamewarmaster.de/software/dhcpclient/
		 *
		 * There are a few other interesting ideas on that page (look under
		 * "Motivation").  Use of netlink events is most interesting.  Think
		 * of various network servers listening for events and reconfiguring.
		 * That would obsolete sending HUP signals and/or make use of restarts.
		 *
		 * Copyright: 2006, 2007 Stefan Rompf <sux@loplof.de>.
		 * License: GPL v2.
		 */
		static const struct sock_filter filter_instr[] = {
			/* load 9th byte (protocol) */
			BPF_STMT(BPF_LD|BPF_B|BPF_ABS, 9),
			/* jump to L1 if it is IPPROTO_UDP, else to L4 */
			BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, IPPROTO_UDP, 0, 6),
			/* L1: load halfword from offset 6 (flags and frag offset) */
			BPF_STMT(BPF_LD|BPF_H|BPF_ABS, 6),
			/* jump to L4 if any bits in frag offset field are set, else to L2 */
			BPF_JUMP(BPF_JMP|BPF_JSET|BPF_K, 0x1fff, 4, 0),
			/* L2: skip IP header (load index reg with header len) */
			BPF_STMT(BPF_LDX|BPF_B|BPF_MSH, 0),
			/* load udp destination port from halfword[header_len + 2] */
			BPF_STMT(BPF_LD|BPF_H|BPF_IND, 2),
			/* jump to L3 if udp dport is CLIENT_PORT, else to L4 */
			BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, 68, 0, 1),
			/* L3: accept packet ("accept 0x7fffffff bytes") */
			/* Accepting 0xffffffff works too but kernel 2.6.19 is buggy */
			BPF_STMT(BPF_RET|BPF_K, 0x7fffffff),
			/* L4: discard packet ("accept zero bytes") */
			BPF_STMT(BPF_RET|BPF_K, 0),
		};
		static const struct sock_fprog filter_prog = {
			.len = sizeof(filter_instr) / sizeof(filter_instr[0]),
			/* casting const away: */
			.filter = (struct sock_filter *) filter_instr,
		};
		/* Ignoring error (kernel may lack support for this) */
		if (setsockopt(fd, SOL_SOCKET, SO_ATTACH_FILTER, &filter_prog,
				sizeof(filter_prog)) >= 0)
			log1("Attached filter to raw socket fd"); // log?
	}
#endif

	if (setsockopt_1(fd, SOL_PACKET, PACKET_AUXDATA) != 0) {
		if (errno != ENOPROTOOPT)
			log1("Can't set PACKET_AUXDATA on raw socket");
	}

	log1("Created raw socket");

	return fd;
}