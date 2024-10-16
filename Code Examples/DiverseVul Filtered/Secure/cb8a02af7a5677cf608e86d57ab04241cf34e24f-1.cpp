static void printPEEROpcodeVersion1(const uint8_t *buf)
{
	char ext_addr[INET6_ADDRSTRLEN];
	char peer_addr[INET6_ADDRSTRLEN];
	syslog(LOG_DEBUG, "PCP PEER: v1 Opcode specific information. \n");
	syslog(LOG_DEBUG, "Protocol: \t\t %d\n", (int)buf[0]);
	syslog(LOG_DEBUG, "Internal port: \t\t %d\n", READNU16(buf + 4));
	syslog(LOG_DEBUG, "External IP: \t\t %s\n", inet_ntop(AF_INET6, buf + 8,
	       ext_addr,INET6_ADDRSTRLEN));
	syslog(LOG_DEBUG, "External port port: \t\t %d\n", READNU16(buf + 6));
	syslog(LOG_DEBUG, "PEER IP: \t\t %s\n", inet_ntop(AF_INET6, buf + 28,
	       peer_addr,INET6_ADDRSTRLEN));
	syslog(LOG_DEBUG, "PEER port port: \t\t %d\n", READNU16(buf + 24));
}