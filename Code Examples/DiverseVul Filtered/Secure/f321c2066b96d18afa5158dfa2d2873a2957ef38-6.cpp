void lease_file_rewrite(void)
{
	int index;
	unsigned short eport, iport;
	int proto;
	char iaddr[32];
	char desc[64];
	char rhost[40];
	unsigned int timestamp;

	if (lease_file == NULL) return;
	remove(lease_file);
	for(index = 0; ; index++) {
		if(get_redirect_rule_by_index(index, 0/*ifname*/, &eport, iaddr, sizeof(iaddr),
		                              &iport, &proto, desc, sizeof(desc),
		                              rhost, sizeof(rhost), &timestamp,
		                              0, 0) < 0)
			break;
		if(lease_file_add(eport, iaddr, iport, proto, desc, timestamp) < 0)
			break;
	}
}