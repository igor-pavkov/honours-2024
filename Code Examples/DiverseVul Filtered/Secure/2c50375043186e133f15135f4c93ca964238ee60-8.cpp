void reload_config(void)
{
	struct Interface *iface;

	flog(LOG_INFO, "attempting to reread config file");

	dlog(LOG_DEBUG, 4, "reopening log");
	if (log_reopen() < 0) {
		perror("log_reopen");
		exit(1);
	}

	iface=IfaceList;
	while(iface)
	{
		struct Interface *next_iface = iface->next;
		struct AdvPrefix *prefix;
		struct AdvRoute *route;
		struct AdvRDNSS *rdnss;
		struct AdvDNSSL *dnssl;

		dlog(LOG_DEBUG, 4, "freeing interface %s", iface->Name);

		prefix = iface->AdvPrefixList;
		while (prefix)
		{
			struct AdvPrefix *next_prefix = prefix->next;

			free(prefix);
			prefix = next_prefix;
		}

		route = iface->AdvRouteList;
		while (route)
		{
			struct AdvRoute *next_route = route->next;

			free(route);
			route = next_route;
		}

		rdnss = iface->AdvRDNSSList;
		while (rdnss)
		{
			struct AdvRDNSS *next_rdnss = rdnss->next;

			free(rdnss);
			rdnss = next_rdnss;
		}

		dnssl = iface->AdvDNSSLList;
		while (dnssl)
		{
			struct AdvDNSSL *next_dnssl = dnssl->next;
			int i;

			for (i = 0; i < dnssl->AdvDNSSLNumber; i++)
				free(dnssl->AdvDNSSLSuffixes[i]);
			free(dnssl->AdvDNSSLSuffixes);
			free(dnssl);

			dnssl = next_dnssl;
		}

		free(iface);
		iface = next_iface;
	}

	IfaceList = NULL;

	/* reread config file */
	if (readin_config(conf_file) < 0) {
		perror("readin_config failed.");
		exit(1);
	}

	/* XXX: fails due to lack of permissions with non-root user */
	config_interface();
	kickoff_adverts();

	flog(LOG_INFO, "resuming normal operation");
}