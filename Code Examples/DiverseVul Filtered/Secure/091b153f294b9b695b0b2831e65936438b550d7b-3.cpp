static int get_ip_port(char *iface, char *ip, const int ip_size)
{
	char *host;
	char *ptr;
	int port = -1;
	struct in_addr addr;

	host = strdup(iface);
	if (!host)
		return -1;

	ptr = strchr(host, ':');
	if (!ptr)
		goto out;

	*ptr++ = 0;

	if (!inet_aton(host, (struct in_addr *)&addr))
		goto out; /* XXX resolve hostname */

	if(strlen(host) > 15)
        {
            port = -1;
            goto out;
        }
	strncpy(ip, host, ip_size);
	port = atoi(ptr);
        if(port <= 0) port = -1;

out:
	free(host);
	return port;
}