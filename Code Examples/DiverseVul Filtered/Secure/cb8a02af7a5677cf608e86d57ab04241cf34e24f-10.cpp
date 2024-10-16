static void printSADSCPOpcode(const uint8_t *buf)
{
	unsigned char sadscp_tol;
	sadscp_tol = buf[12];	/* tolerance_fields */

	syslog(LOG_DEBUG, "PCP SADSCP: Opcode specific information.\n");
	syslog(LOG_DEBUG, "Delay tolerance %d \n", (sadscp_tol>>6)&3);
	syslog(LOG_DEBUG, "Loss tolerance %d \n",  (sadscp_tol>>4)&3);
	syslog(LOG_DEBUG, "Jitter tolerance %d \n",  (sadscp_tol>>2)&3);
	syslog(LOG_DEBUG, "RRR %d \n", sadscp_tol&3);
	syslog(LOG_DEBUG, "AppName Length %d \n", buf[13]);
	syslog(LOG_DEBUG, "Application name %.*s \n", buf[13], buf + 14);
}