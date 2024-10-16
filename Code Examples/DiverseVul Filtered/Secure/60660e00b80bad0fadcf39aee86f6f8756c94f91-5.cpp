static int get_gateway_host_ip(struct tunnel *tunnel)
{
	const struct addrinfo hints = { .ai_family = AF_INET };
	struct addrinfo *result = NULL;

	int ret = getaddrinfo(tunnel->config->gateway_host, NULL, &hints, &result);

	if (ret) {
		if (ret == EAI_SYSTEM)
			log_error("getaddrinfo: %s\n", strerror(errno));
		else
			log_error("getaddrinfo: %s\n", gai_strerror(ret));
		return 1;
	}

	tunnel->config->gateway_ip = ((struct sockaddr_in *)
	                              result->ai_addr)->sin_addr;
	freeaddrinfo(result);

	setenv("VPN_GATEWAY", inet_ntoa(tunnel->config->gateway_ip), 0);

	return 0;
}