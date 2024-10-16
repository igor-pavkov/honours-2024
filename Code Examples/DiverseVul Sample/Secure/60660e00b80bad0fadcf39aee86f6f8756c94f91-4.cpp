int run_tunnel(struct vpn_config *config)
{
	int ret;
	struct tunnel tunnel = {
		.config = config,
		.state = STATE_DOWN,
		.ssl_context = NULL,
		.ssl_handle = NULL,
		.ipv4.ns1_addr.s_addr = 0,
		.ipv4.ns2_addr.s_addr = 0,
		.ipv4.dns_suffix = NULL,
		.on_ppp_if_up = on_ppp_if_up,
		.on_ppp_if_down = on_ppp_if_down
	};

	// Step 0: get gateway host IP
	log_debug("Resolving gateway host ip\n");
	ret = get_gateway_host_ip(&tunnel);
	if (ret)
		goto err_tunnel;

	// Step 1: open a SSL connection to the gateway
	log_debug("Establishing ssl connection\n");
	ret = ssl_connect(&tunnel);
	if (ret)
		goto err_tunnel;
	log_info("Connected to gateway.\n");

	// Step 2: connect to the HTTP interface and authenticate to get a
	// cookie
	ret = auth_log_in(&tunnel);
	if (ret != 1) {
		log_error("Could not authenticate to gateway. Please check the password, client certificate, etc.\n");
		log_debug("%s %d\n", err_http_str(ret), ret);
		ret = 1;
		goto err_tunnel;
	}
	log_info("Authenticated.\n");
	log_debug("Cookie: %s\n", tunnel.cookie);

	ret = auth_request_vpn_allocation(&tunnel);
	if (ret != 1) {
		log_error("VPN allocation request failed (%s).\n",
		          err_http_str(ret));
		ret = 1;
		goto err_tunnel;
	}
	log_info("Remote gateway has allocated a VPN.\n");

	ret = ssl_connect(&tunnel);
	if (ret)
		goto err_tunnel;

	// Step 3: get configuration
	log_debug("Retrieving configuration\n");
	ret = auth_get_config(&tunnel);
	if (ret != 1) {
		log_error("Could not get VPN configuration (%s).\n",
		          err_http_str(ret));
		ret = 1;
		goto err_tunnel;
	}

	// Step 4: run a pppd process
	log_debug("Establishing the tunnel\n");
	ret = pppd_run(&tunnel);
	if (ret)
		goto err_tunnel;

	// Step 5: ask gateway to start tunneling
	log_debug("Switch to tunneling mode\n");
	ret = http_send(&tunnel,
	                "GET /remote/sslvpn-tunnel HTTP/1.1\r\n"
	                "Host: sslvpn\r\n"
	                "Cookie: %s\r\n\r\n",
	                tunnel.cookie);
	if (ret != 1) {
		log_error("Could not start tunnel (%s).\n", err_http_str(ret));
		ret = 1;
		goto err_start_tunnel;
	}

	tunnel.state = STATE_CONNECTING;
	ret = 0;

	// Step 6: perform io between pppd and the gateway, while tunnel is up
	log_debug("Starting IO through the tunnel\n");
	io_loop(&tunnel);

	log_debug("disconnecting\n");
	if (tunnel.state == STATE_UP)
		if (tunnel.on_ppp_if_down != NULL)
			tunnel.on_ppp_if_down(&tunnel);

	tunnel.state = STATE_DISCONNECTING;

err_start_tunnel:
	pppd_terminate(&tunnel);
	log_info("Terminated %s.\n", PPP_DAEMON);
err_tunnel:
	log_info("Closed connection to gateway.\n");
	tunnel.state = STATE_DOWN;

	if (ssl_connect(&tunnel)) {
		log_info("Could not log out.\n");
	} else {
		auth_log_out(&tunnel);
		log_info("Logged out.\n");
	}

	// explicitly free the buffer allocated for split routes of the ipv4 config
	if (tunnel.ipv4.split_rt != NULL) {
		free(tunnel.ipv4.split_rt);
		tunnel.ipv4.split_rt = NULL;
	}
	return ret;
}