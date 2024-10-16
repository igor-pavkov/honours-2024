static void load_config(void)
{
	char *opt;

	opt = conf_get_opt("pptp", "timeout");
	if (opt && atoi(opt) > 0)
		conf_timeout = atoi(opt);

	opt = conf_get_opt("pptp", "echo-interval");
	if (opt && atoi(opt) >= 0)
		conf_echo_interval = atoi(opt);

	opt = conf_get_opt("pptp", "echo-failure");
	if (opt && atoi(opt) >= 0)
		conf_echo_failure = atoi(opt);

	opt = conf_get_opt("pptp", "verbose");
	if (opt && atoi(opt) >= 0)
		conf_verbose = atoi(opt) > 0;

	opt = conf_get_opt("pptp", "ppp-max-mtu");
	if (opt && atoi(opt) > 0)
		conf_ppp_max_mtu = atoi(opt);
	else
		conf_ppp_max_mtu = PPTP_MAX_MTU;

	conf_mppe = MPPE_UNSET;
	opt = conf_get_opt("pptp", "mppe");
	if (opt) {
		if (strcmp(opt, "deny") == 0)
			conf_mppe = MPPE_DENY;
		else if (strcmp(opt, "allow") == 0)
			conf_mppe = MPPE_ALLOW;
		else if (strcmp(opt, "prefer") == 0)
			conf_mppe = MPPE_PREFER;
		else if (strcmp(opt, "require") == 0)
			conf_mppe = MPPE_REQUIRE;
	}

	conf_ip_pool = conf_get_opt("pptp", "ip-pool");
	conf_ipv6_pool = conf_get_opt("pptp", "ipv6-pool");
	conf_dpv6_pool = conf_get_opt("pptp", "ipv6-pool-delegate");
	conf_ifname = conf_get_opt("pptp", "ifname");

	switch (iprange_check_activation()) {
	case IPRANGE_DISABLED:
		log_warn("pptp: iprange module disabled, improper IP configuration of PPP interfaces may cause kernel soft lockup\n");
		break;
	case IPRANGE_NO_RANGE:
		log_warn("pptp: no IP address range defined in section [%s], incoming PPTP connections will be rejected\n",
			 IPRANGE_CONF_SECTION);
		break;
	default:
		/* Makes compiler happy */
		break;
	}
}