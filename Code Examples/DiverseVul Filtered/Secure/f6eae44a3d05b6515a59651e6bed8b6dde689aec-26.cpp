static int parse_unc(const char *unc_name, struct parsed_mount_info *parsed_info)
{
	int length = strnlen(unc_name, MAX_UNC_LEN);
	const char *host, *share, *prepath;
	size_t hostlen, sharelen, prepathlen;

	if (length > (MAX_UNC_LEN - 1)) {
		fprintf(stderr, "mount error: UNC name too long\n");
		return EX_USAGE;
	}

	if (length < 3) {
		fprintf(stderr, "mount error: UNC name too short\n");
		return EX_USAGE;
	}

	if ((strncasecmp("cifs://", unc_name, 7) == 0) ||
	    (strncasecmp("smb://", unc_name, 6) == 0)) {
		fprintf(stderr,
			"Mounting cifs URL not implemented yet. Attempt to mount %s\n",
			unc_name);
		return EX_USAGE;
	}

	/* Set up "host" and "share" pointers based on UNC format. */
	if (strncmp(unc_name, "//", 2) && strncmp(unc_name, "\\\\", 2)) {
		/*
		 * check for nfs syntax (server:/share/prepath)
		 *
		 * FIXME: IPv6 addresses?
		 */
		host = unc_name;
		share = strchr(host, ':');
		if (!share) {
			fprintf(stderr, "mount.cifs: bad UNC (%s)\n", unc_name);
			return EX_USAGE;
		}
		hostlen = share - host;
		share++;
		if (*share == '/')
			++share;
	} else {
		host = unc_name + 2;
		hostlen = strcspn(host, "/\\");
		if (!hostlen) {
			fprintf(stderr, "mount.cifs: bad UNC (%s)\n", unc_name);
			return EX_USAGE;
		}
		share = host + hostlen + 1;
	}

	if (hostlen + 1 > sizeof(parsed_info->host)) {
		fprintf(stderr, "mount.cifs: host portion of UNC too long\n");
		return EX_USAGE;
	}

	sharelen = strcspn(share, "/\\");
	if (sharelen + 1 > sizeof(parsed_info->share)) {
		fprintf(stderr, "mount.cifs: share portion of UNC too long\n");
		return EX_USAGE;
	}

	prepath = share + sharelen;
	if (*prepath != '\0')
		prepath++;

	prepathlen = strlen(prepath);

	if (prepathlen + 1 > sizeof(parsed_info->prefix)) {
		fprintf(stderr, "mount.cifs: UNC prefixpath too long\n");
		return EX_USAGE;
	}

	/* copy pieces into their resepective buffers */
	memcpy(parsed_info->host, host, hostlen);
	memcpy(parsed_info->share, share, sharelen);
	memcpy(parsed_info->prefix, prepath, prepathlen);

	return 0;
}