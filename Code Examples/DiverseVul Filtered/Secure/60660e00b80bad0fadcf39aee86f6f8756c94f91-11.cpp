static int tcp_connect(struct tunnel *tunnel)
{
	int ret, handle;
	struct sockaddr_in server;
	char *env_proxy;

	handle = socket(AF_INET, SOCK_STREAM, 0);
	if (handle == -1) {
		log_error("socket: %s\n", strerror(errno));
		goto err_socket;
	}
	env_proxy = getenv("https_proxy");
	if (env_proxy == NULL)
		env_proxy = getenv("HTTPS_PROXY");
	if (env_proxy == NULL)
		env_proxy = getenv("all_proxy");
	if (env_proxy == NULL)
		env_proxy = getenv("ALL_PROXY");
	if (env_proxy != NULL) {
		char *proxy_host, *proxy_port;
		// protect the original environment from modifications
		env_proxy = strdup(env_proxy);
		if (env_proxy == NULL) {
			log_error("strdup: %s\n", strerror(errno));
			goto err_strdup;
		}
		// get rid of a trailing slash
		if (*env_proxy && env_proxy[strlen(env_proxy) - 1] == '/')
			env_proxy[strlen(env_proxy) - 1] = '\0';
		// get rid of a http(s):// prefix in env_proxy
		proxy_host = strstr(env_proxy, "://");
		if (proxy_host == NULL)
			proxy_host = env_proxy;
		else
			proxy_host += 3;
		// split host and port
		proxy_port = index(proxy_host, ':');
		if (proxy_port != NULL) {
			proxy_port[0] = '\0';
			proxy_port++;
			server.sin_port = htons(strtoul(proxy_port, NULL, 10));
		} else {
			server.sin_port = htons(tunnel->config->gateway_port);
		}
		// get rid of a trailing slash
		if (*proxy_host && proxy_host[strlen(proxy_host) - 1] == '/')
			proxy_host[strlen(proxy_host) - 1] = '\0';
		log_debug("proxy_host: %s\n", proxy_host);
		log_debug("proxy_port: %s\n", proxy_port);
		server.sin_addr.s_addr = inet_addr(proxy_host);
		// if host is given as a FQDN we have to do a DNS lookup
		if (server.sin_addr.s_addr == INADDR_NONE) {
			const struct addrinfo hints = { .ai_family = AF_INET };
			struct addrinfo *result = NULL;

			ret = getaddrinfo(proxy_host, NULL, &hints, &result);
			if (ret) {
				if (ret == EAI_SYSTEM)
					log_error("getaddrinfo: %s\n", strerror(errno));
				else
					log_error("getaddrinfo: %s\n", gai_strerror(ret));
				goto err_connect;
			}

			server.sin_addr = ((struct sockaddr_in *)
			                   result->ai_addr)->sin_addr;
			freeaddrinfo(result);
		}
	} else {
		server.sin_port = htons(tunnel->config->gateway_port);
		server.sin_addr = tunnel->config->gateway_ip;
	}

	log_debug("server_addr: %s\n", inet_ntoa(server.sin_addr));
	log_debug("server_port: %u\n", ntohs(server.sin_port));
	server.sin_family = AF_INET;
	memset(&(server.sin_zero), '\0', 8);
	log_debug("gateway_addr: %s\n", inet_ntoa(tunnel->config->gateway_ip));
	log_debug("gateway_port: %u\n", tunnel->config->gateway_port);

	ret = connect(handle, (struct sockaddr *) &server, sizeof(server));
	if (ret) {
		log_error("connect: %s\n", strerror(errno));
		goto err_connect;
	}

	if (env_proxy != NULL) {
		char request[128];

		// https://tools.ietf.org/html/rfc7231#section-4.3.6
		sprintf(request, "CONNECT %s:%u HTTP/1.1\r\nHost: %s:%u\r\n\r\n",
		        inet_ntoa(tunnel->config->gateway_ip),
		        tunnel->config->gateway_port,
		        inet_ntoa(tunnel->config->gateway_ip),
		        tunnel->config->gateway_port);
		ssize_t bytes_written = write(handle, request, strlen(request));
		if (bytes_written != strlen(request)) {
			log_error("write error while talking to proxy: %s\n",
			          strerror(errno));
			goto err_connect;
		}

		// wait for a "200 OK" reply from the proxy,
		// be careful not to fetch too many bytes at once
		const char *response = NULL;

		memset(&(request), '\0', sizeof(request));
		for (int j = 0; response == NULL; j++) {
			/*
			 * Coverity detected a defect:
			 *  CID 200508: String not null terminated (STRING_NULL)
			 *
			 * It is actually a false positive:
			 * • Function memset() initializes 'request' with '\0'
			 * • Function read() gets a single char into: request[j]
			 * • The final '\0' cannot be overwritten because:
			 *   	j < ARRAY_SIZE(request) - 1
			 */
			ssize_t bytes_read = read(handle, &(request[j]), 1);
			if (bytes_read < 1) {
				log_error("Proxy response is unexpectedly large and cannot fit in the %lu-bytes buffer.\n",
				          ARRAY_SIZE(request));
				goto err_proxy_response;
			}

			// detect "200"
			static const char HTTP_STATUS_200[] = "200";
			response = strstr(request, HTTP_STATUS_200);

			// detect end-of-line after "200"
			if (response != NULL) {
				/*
				 * RFC2616 states in section 2.2 Basic Rules:
				 * 	CR     = <US-ASCII CR, carriage return (13)>
				 * 	LF     = <US-ASCII LF, linefeed (10)>
				 * 	HTTP/1.1 defines the sequence CR LF as the
				 * 	end-of-line marker for all protocol elements
				 * 	except the entity-body (see appendix 19.3
				 * 	for tolerant applications).
				 * 		CRLF   = CR LF
				 *
				 * RFC2616 states in section 19.3 Tolerant Applications:
				 * 	The line terminator for message-header fields
				 * 	is the sequence CRLF. However, we recommend
				 * 	that applications, when parsing such headers,
				 * 	recognize a single LF as a line terminator
				 * 	and ignore the leading CR.
				 */
				static const char *const HTTP_EOL[] = {
					"\r\n\r\n",
					"\n\n"
				};
				const char *eol = NULL;
				for (int i = 0; (i < ARRAY_SIZE(HTTP_EOL)) &&
				     (eol == NULL); i++)
					eol = strstr(response, HTTP_EOL[i]);
				response = eol;
			}

			if (j > ARRAY_SIZE(request) - 2) {
				log_error("Proxy response does not contain \"%s\" as expected.\n",
				          HTTP_STATUS_200);
				goto err_proxy_response;
			}
		}

		free(env_proxy); // release memory allocated by strdup()
	}

	return handle;

err_proxy_response:
err_connect:
	free(env_proxy); // release memory allocated by strdup()
err_strdup:
	close(handle);
err_socket:
	return -1;
}