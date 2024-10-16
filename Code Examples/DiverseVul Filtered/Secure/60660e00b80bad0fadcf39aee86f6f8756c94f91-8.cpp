static int on_ppp_if_up(struct tunnel *tunnel)
{
	log_info("Interface %s is UP.\n", tunnel->ppp_iface);

	if (tunnel->config->set_routes) {
		int ret;

		log_info("Setting new routes...\n");

		ret = ipv4_set_tunnel_routes(tunnel);

		if (ret != 0)
			log_warn("Adding route table is incomplete. Please check route table.\n");
	}

	if (tunnel->config->set_dns) {
		log_info("Adding VPN nameservers...\n");
		ipv4_add_nameservers_to_resolv_conf(tunnel);
	}

	log_info("Tunnel is up and running.\n");

#if HAVE_SYSTEMD
	sd_notify(0, "READY=1");
#endif

	return 0;
}