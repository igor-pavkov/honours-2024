static void add_client_options(struct dhcp_packet *packet)
{
	int i, end, len;

	udhcp_add_simple_option(packet, DHCP_MAX_SIZE, htons(IP_UDP_DHCP_SIZE));

	/* Add a "param req" option with the list of options we'd like to have
	 * from stubborn DHCP servers. Pull the data from the struct in common.c.
	 * No bounds checking because it goes towards the head of the packet. */
	end = udhcp_end_option(packet->options);
	len = 0;
	for (i = 1; i < DHCP_END; i++) {
		if (client_config.opt_mask[i >> 3] & (1 << (i & 7))) {
			packet->options[end + OPT_DATA + len] = i;
			len++;
		}
	}
	if (len) {
		packet->options[end + OPT_CODE] = DHCP_PARAM_REQ;
		packet->options[end + OPT_LEN] = len;
		packet->options[end + OPT_DATA + len] = DHCP_END;
	}

	if (client_config.vendorclass)
		udhcp_add_binary_option(packet, client_config.vendorclass);
	if (client_config.hostname)
		udhcp_add_binary_option(packet, client_config.hostname);
	if (client_config.fqdn)
		udhcp_add_binary_option(packet, client_config.fqdn);

	/* Request broadcast replies if we have no IP addr */
	if ((option_mask32 & OPT_B) && packet->ciaddr == 0)
		packet->flags |= htons(BROADCAST_FLAG);

	/* Add -x options if any */
	{
		struct option_set *curr = client_config.options;
		while (curr) {
			udhcp_add_binary_option(packet, curr->data);
			curr = curr->next;
		}
//		if (client_config.sname)
//			strncpy((char*)packet->sname, client_config.sname, sizeof(packet->sname) - 1);
//		if (client_config.boot_file)
//			strncpy((char*)packet->file, client_config.boot_file, sizeof(packet->file) - 1);
	}

	// This will be needed if we remove -V VENDOR_STR in favor of
	// -x vendor:VENDOR_STR
	//if (!udhcp_find_option(packet.options, DHCP_VENDOR))
	//	/* not set, set the default vendor ID */
	//	...add (DHCP_VENDOR, "udhcp "BB_VER) opt...
}