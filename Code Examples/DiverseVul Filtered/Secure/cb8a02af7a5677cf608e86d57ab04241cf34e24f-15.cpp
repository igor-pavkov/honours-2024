static void printPEEROpcodeVersion2(const uint8_t *buf)
{
	char ext_addr[INET6_ADDRSTRLEN];
	char peer_addr[INET6_ADDRSTRLEN];

	syslog(LOG_DEBUG, "PCP PEER: v2 Opcode specific information.");
	syslog(LOG_DEBUG, "nonce:        \t%08x%08x%08x",
	       READNU32(buf), READNU32(buf+4), READNU32(buf+8));
	syslog(LOG_DEBUG, "Protocol:     \t%d", buf[12]);
	syslog(LOG_DEBUG, "Internal port:\t%d", READNU16(buf + 16));
	syslog(LOG_DEBUG, "External IP:  \t%s", inet_ntop(AF_INET6, buf + 20,
	       ext_addr, INET6_ADDRSTRLEN));
	syslog(LOG_DEBUG, "External port:\t%d", READNU16(buf + 18));
	syslog(LOG_DEBUG, "PEER IP:      \t%s", inet_ntop(AF_INET6, buf + 40,
	       peer_addr, INET6_ADDRSTRLEN));
	syslog(LOG_DEBUG, "PEER port:    \t%d", READNU16(buf + 36));
}