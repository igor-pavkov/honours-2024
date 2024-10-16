static void aun_send_response(__u32 addr, unsigned long seq, int code, int cb)
{
	struct sockaddr_in sin = {
		.sin_family = AF_INET,
		.sin_port = htons(AUN_PORT),
		.sin_addr = {.s_addr = addr}
	};
	struct aunhdr ah = {.code = code, .cb = cb, .handle = seq};
	struct kvec iov = {.iov_base = (void *)&ah, .iov_len = sizeof(ah)};
	struct msghdr udpmsg;

	udpmsg.msg_name = (void *)&sin;
	udpmsg.msg_namelen = sizeof(sin);
	udpmsg.msg_control = NULL;
	udpmsg.msg_controllen = 0;
	udpmsg.msg_flags=0;

	kernel_sendmsg(udpsock, &udpmsg, &iov, 1, sizeof(ah));
}