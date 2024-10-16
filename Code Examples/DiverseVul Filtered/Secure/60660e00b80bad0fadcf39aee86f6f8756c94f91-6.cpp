static int on_ppp_if_down(struct tunnel *tunnel)
{
	log_info("Setting ppp interface down.\n");

	if (tunnel->config->set_routes) {
		log_info("Restoring routes...\n");
		ipv4_restore_routes(tunnel);
	}

	if (tunnel->config->set_dns) {
		log_info("Removing VPN nameservers...\n");
		ipv4_del_nameservers_from_resolv_conf(tunnel);
	}

	return 0;
}