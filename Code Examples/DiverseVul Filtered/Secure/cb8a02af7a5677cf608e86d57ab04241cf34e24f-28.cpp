static void printMAPOpcodeVersion2(const uint8_t *buf)
{
	char map_addr[INET6_ADDRSTRLEN];
	syslog(LOG_DEBUG, "PCP MAP: v2 Opcode specific information.");
	syslog(LOG_DEBUG, "MAP nonce:   \t%08x%08x%08x",
	       READNU32(buf), READNU32(buf+4), READNU32(buf+8));
	syslog(LOG_DEBUG, "MAP protocol:\t%d", (int)buf[12]);
	syslog(LOG_DEBUG, "MAP int port:\t%d", (int)READNU16(buf+16));
	syslog(LOG_DEBUG, "MAP ext port:\t%d", (int)READNU16(buf+18));
	syslog(LOG_DEBUG, "MAP Ext IP:  \t%s", inet_ntop(AF_INET6,
	       buf+20, map_addr, INET6_ADDRSTRLEN));
}