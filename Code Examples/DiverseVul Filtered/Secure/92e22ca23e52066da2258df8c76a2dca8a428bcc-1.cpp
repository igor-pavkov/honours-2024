int setup_linklocal_addr(struct Interface *iface)
{
	FILE *fp;
	char str_addr[40];
	unsigned int plen, scope, dad_status, if_idx;
	char devname[IFNAMSIZ];

	if ((fp = fopen(PATH_PROC_NET_IF_INET6, "r")) == NULL)
	{
		flog(LOG_ERR, "can't open %s: %s", PATH_PROC_NET_IF_INET6,
			strerror(errno));
		return (-1);
	}

	while (fscanf(fp, "%32s %x %02x %02x %02x %15s\n",
		      str_addr, &if_idx, &plen, &scope, &dad_status,
		      devname) != EOF)
	{
		if (scope == IPV6_ADDR_LINKLOCAL &&
		    strcmp(devname, iface->Name) == 0)
		{
			struct in6_addr addr;
			unsigned int ap;
			int i;

			for (i=0; i<16; i++)
			{
				sscanf(str_addr + i * 2, "%02x", &ap);
				addr.s6_addr[i] = (unsigned char)ap;
			}
			memcpy(&iface->if_addr, &addr, sizeof(iface->if_addr));

			iface->if_index = if_idx;
			fclose(fp);
			return 0;
		}
	}

	flog(LOG_ERR, "no linklocal address configured for %s", iface->Name);
	fclose(fp);
	return (-1);
}