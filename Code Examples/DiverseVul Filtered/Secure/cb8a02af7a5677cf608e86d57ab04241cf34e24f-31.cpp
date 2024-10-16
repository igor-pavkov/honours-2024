static void printMAPOpcodeVersion1(const uint8_t *buf)
{
	char map_addr[INET6_ADDRSTRLEN];
	syslog(LOG_DEBUG, "PCP MAP: v1 Opcode specific information. \n");
	syslog(LOG_DEBUG, "MAP protocol: \t\t %d\n", (int)buf[0] );
	syslog(LOG_DEBUG, "MAP int port: \t\t %d\n", (int)READNU16(buf+4));
	syslog(LOG_DEBUG, "MAP ext port: \t\t %d\n", (int)READNU16(buf+6));
	syslog(LOG_DEBUG, "MAP Ext IP: \t\t %s\n", inet_ntop(AF_INET6,
	       buf+8, map_addr, INET6_ADDRSTRLEN));
}