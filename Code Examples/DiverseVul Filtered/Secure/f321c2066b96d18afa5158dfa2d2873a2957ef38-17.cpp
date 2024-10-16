int reload_from_lease_file()
{
	FILE * fd;
	char * p;
	unsigned short eport, iport;
	char * proto;
	char * iaddr;
	char * desc;
	char * rhost;
	unsigned int leaseduration;
	unsigned int timestamp;
	time_t current_time;
#ifndef LEASEFILE_USE_REMAINING_TIME
	time_t current_unix_time;
#endif
	char line[128];
	int r;

	if(!lease_file) return -1;
	fd = fopen( lease_file, "r");
	if (fd==NULL) {
		syslog(LOG_ERR, "could not open lease file: %s", lease_file);
		return -1;
	}
	if(unlink(lease_file) < 0) {
		syslog(LOG_WARNING, "could not unlink file %s : %m", lease_file);
	}

	current_time = upnp_time();
#ifndef LEASEFILE_USE_REMAINING_TIME
	current_unix_time = time(NULL);
#endif
	while(fgets(line, sizeof(line), fd)) {
		syslog(LOG_DEBUG, "parsing lease file line '%s'", line);
		proto = line;
		p = strchr(line, ':');
		if(!p) {
			syslog(LOG_ERR, "unrecognized data in lease file");
			continue;
		}
		*(p++) = '\0';
		iaddr = strchr(p, ':');
		if(!iaddr) {
			syslog(LOG_ERR, "unrecognized data in lease file");
			continue;
		}
		*(iaddr++) = '\0';
		eport = (unsigned short)atoi(p);
		p = strchr(iaddr, ':');
		if(!p) {
			syslog(LOG_ERR, "unrecognized data in lease file");
			continue;
		}
		*(p++) = '\0';
		iport = (unsigned short)atoi(p);
		p = strchr(p, ':');
		if(!p) {
			syslog(LOG_ERR, "unrecognized data in lease file");
			continue;
		}
		*(p++) = '\0';
		desc = strchr(p, ':');
		if(!desc) {
			syslog(LOG_ERR, "unrecognized data in lease file");
			continue;
		}
		*(desc++) = '\0';
		/*timestamp = (unsigned int)atoi(p);*/
		timestamp = (unsigned int)strtoul(p, NULL, 10);
		/* trim description */
		while(isspace(*desc))
			desc++;
		p = desc;
		while(*(p+1))
			p++;
		while(isspace(*p) && (p > desc))
			*(p--) = '\0';

		if(timestamp > 0) {
#ifdef LEASEFILE_USE_REMAINING_TIME
			leaseduration = timestamp;
			timestamp += current_time;	/* convert to our time */
#else
			if(timestamp <= (unsigned int)current_unix_time) {
				syslog(LOG_NOTICE, "already expired lease in lease file");
				continue;
			} else {
				leaseduration = timestamp - current_unix_time;
				timestamp = leaseduration + current_time; /* convert to our time */
			}
#endif
		} else {
			leaseduration = 0;	/* default value */
		}
		rhost = NULL;
		r = upnp_redirect(rhost, eport, iaddr, iport, proto, desc, leaseduration);
		if(r == -1) {
			syslog(LOG_ERR, "Failed to redirect %hu -> %s:%hu protocol %s",
			       eport, iaddr, iport, proto);
		} else if(r == -2) {
			/* Add the redirection again to the lease file */
			lease_file_add(eport, iaddr, iport, proto_atoi(proto),
			               desc, timestamp);
		}
	}
	fclose(fd);

	return 0;
}