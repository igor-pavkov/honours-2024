static u32 do_write_udp()
{
	GF_Err e;
	GF_Socket *sock = gf_sk_new(GF_SOCK_TYPE_UDP);
	u16 port = 2345;
	char *sep = strrchr(udp_dest, ':');
	if (sep) {
		sep[0] = 0;
		port = atoi(sep+1);
	}
	e = gf_sk_bind( sock, "127.0.0.1", 0, udp_dest, port, 0);
	if (sep) sep[0] = ':';
	if (e) {
		M4_LOG(GF_LOG_ERROR, ("Failed to bind socket to %s: %s\n", udp_dest, gf_error_to_string(e) ));
	} else {
		e = gf_sk_send(sock, (u8 *) inName, (u32)strlen(inName));
		if (e)
			M4_LOG(GF_LOG_ERROR, ("Failed to send datagram: %s\n", gf_error_to_string(e) ));
	}
	gf_sk_del(sock);
	return 0;
}