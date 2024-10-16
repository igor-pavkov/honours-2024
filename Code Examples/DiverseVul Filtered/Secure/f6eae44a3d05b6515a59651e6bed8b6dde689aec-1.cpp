int main(int argc, char **argv)
{
	int c;
	char *orgoptions = NULL;
	char *mountpoint = NULL;
	char *options = NULL;
	char *orig_dev = NULL;
	char *currentaddress, *nextaddress;
	int rc = 0;
	int already_uppercased = 0;
	size_t options_size = MAX_OPTIONS_LEN;
	struct parsed_mount_info *parsed_info = NULL;
	pid_t pid;
	const char *fstype;

	rc = check_setuid();
	if (rc)
		return rc;

	rc = drop_capabilities(1);
	if (rc)
		return EX_SYSERR;

	/* setlocale(LC_ALL, "");
	   bindtextdomain(PACKAGE, LOCALEDIR);
	   textdomain(PACKAGE); */

	if (!argc || !argv) {
		rc = mount_usage(stderr);
		goto mount_exit;
	}

	thisprogram = basename(argv[0]);
	if (thisprogram == NULL)
		thisprogram = "mount.cifs";

	/* allocate parsed_info as shared anonymous memory range */
	parsed_info = mmap(0, sizeof(*parsed_info), PROT_READ | PROT_WRITE,
			   MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if (parsed_info == (struct parsed_mount_info *) -1) {
		parsed_info = NULL;
		fprintf(stderr, "Unable to allocate memory: %s\n",
				strerror(errno));
		return EX_SYSERR;
	}

	/* add sharename in opts string as unc= parm */
	while ((c = getopt_long(argc, argv, "?fhno:rvVw",
				longopts, NULL)) != -1) {
		switch (c) {
		case '?':
		case 'h':	/* help */
			rc = mount_usage(stdout);
			goto mount_exit;
		case 'n':
			++parsed_info->nomtab;
			break;
		case 'o':
			orgoptions = strndup(optarg, MAX_OPTIONS_LEN);
			if (!orgoptions) {
				rc = EX_SYSERR;
				goto mount_exit;
			}
			break;
		case 'r':	/* mount readonly */
			parsed_info->flags |= MS_RDONLY;
			break;
		case 'v':
			++parsed_info->verboseflag;
			break;
		case 'V':
			print_cifs_mount_version();
			exit(0);
		case 'w':
			parsed_info->flags &= ~MS_RDONLY;
			break;
		case 'f':
			++parsed_info->fakemnt;
			break;
		default:
			fprintf(stderr, "unknown command-line option: %c\n", c);
			rc = mount_usage(stderr);
			goto mount_exit;
		}
	}

	if (argc < 3 || argv[optind] == NULL || argv[optind + 1] == NULL) {
		rc = mount_usage(stderr);
		goto mount_exit;
	}

	orig_dev = argv[optind];
	mountpoint = argv[optind + 1];

	/* chdir into mountpoint as soon as possible */
	rc = toggle_dac_capability(0, 1);
	if (rc)
		return rc;
	rc = chdir(mountpoint);
	if (rc) {
		fprintf(stderr, "Couldn't chdir to %s: %s\n", mountpoint,
			strerror(errno));
		rc = EX_USAGE;
		goto mount_exit;
	}

	mountpoint = realpath(".", NULL);
	if (!mountpoint) {
		fprintf(stderr, "Unable to resolve %s to canonical path: %s\n",
			mountpoint, strerror(errno));
		rc = EX_SYSERR;
		goto mount_exit;
	}
	rc = toggle_dac_capability(0, 0);
	if (rc)
		return rc;

	/*
	 * mount.cifs does privilege separation. Most of the code to handle
	 * assembling the mount info is done in a child process that drops
	 * privileges. The info is assembled in parsed_info which is a
	 * shared, mmaped memory segment. The parent waits for the child to
	 * exit and checks the return code. If it's anything but "0", then
	 * the process exits without attempting anything further.
	 */
	pid = fork();
	if (pid == -1) {
		fprintf(stderr, "Unable to fork: %s\n", strerror(errno));
		rc = EX_SYSERR;
		goto mount_exit;
	} else if (!pid) {
		/* child */
		rc = assemble_mountinfo(parsed_info, thisprogram, mountpoint,
					orig_dev, orgoptions);
		return rc;
	} else {
		/* parent */
		pid = wait(&rc);
		if (!WIFEXITED(rc)) {
			fprintf(stderr, "Child process terminated abnormally.\n");
			rc = EX_SYSERR;
			goto mount_exit;
		}
		rc = WEXITSTATUS(rc);
		if (rc)
			goto mount_exit;
	}

	options = calloc(options_size, 1);
	if (!options) {
		fprintf(stderr, "Unable to allocate memory.\n");
		rc = EX_SYSERR;
		goto mount_exit;
	}

	currentaddress = parsed_info->addrlist;
	nextaddress = strchr(currentaddress, ',');
	if (nextaddress)
		*nextaddress++ = '\0';

mount_retry:
	if (!currentaddress) {
		fprintf(stderr, "Unable to find suitable address.\n");
		rc = EX_SYSERR;
		goto mount_exit;
	}
	strlcpy(options, "ip=", options_size);
	strlcat(options, currentaddress, options_size);

	strlcat(options, ",unc=\\\\", options_size);
	strlcat(options, parsed_info->host, options_size);
	strlcat(options, "\\", options_size);
	strlcat(options, parsed_info->share, options_size);

	if (*parsed_info->options) {
		strlcat(options, ",", options_size);
		strlcat(options, parsed_info->options, options_size);
	}

	if (*parsed_info->prefix) {
		strlcat(options, ",prefixpath=", options_size);
		strlcat(options, parsed_info->prefix, options_size);
	}

	if (parsed_info->verboseflag)
		fprintf(stderr, "%s kernel mount options: %s",
			thisprogram, options);

	if (parsed_info->got_password) {
		/*
		 * Commas have to be doubled, or else they will
		 * look like the parameter separator
		 */
		strlcat(options, ",pass=", options_size);
		strlcat(options, parsed_info->password, options_size);
		if (parsed_info->verboseflag)
			fprintf(stderr, ",pass=********");
	}

	if (parsed_info->verboseflag)
		fprintf(stderr, "\n");

	rc = check_mtab(thisprogram, orig_dev, mountpoint);
	if (rc)
		goto mount_exit;

	if (strcmp(thisprogram, "mount.smb2") == 0)
		fstype = smb2_fstype;
	else
		fstype = cifs_fstype;

	if (!parsed_info->fakemnt) {
		toggle_dac_capability(0, 1);
		rc = mount(orig_dev, ".", fstype, parsed_info->flags, options);
		toggle_dac_capability(0, 0);
		if (rc == 0)
			goto do_mtab;

		switch (errno) {
		case ECONNREFUSED:
		case EHOSTUNREACH:
			currentaddress = nextaddress;
			if (currentaddress) {
				nextaddress = strchr(currentaddress, ',');
				if (nextaddress)
					*nextaddress++ = '\0';
			}
			goto mount_retry;
		case ENODEV:
			fprintf(stderr,
				"mount error: %s filesystem not supported by the system\n", fstype);
			break;
		case ENXIO:
			if (!already_uppercased &&
			    uppercase_string(parsed_info->host) &&
			    uppercase_string(parsed_info->share) &&
			    uppercase_string(parsed_info->prefix)) {
				fprintf(stderr,
					"Retrying with upper case share name\n");
				already_uppercased = 1;
				goto mount_retry;
			}
		}
		fprintf(stderr, "mount error(%d): %s\n", errno,
			strerror(errno));
		fprintf(stderr,
			"Refer to the %s(8) manual page (e.g. man "
			"%s)\n", thisprogram, thisprogram);
		rc = EX_FAIL;
		goto mount_exit;
	}

do_mtab:
	if (!parsed_info->nomtab && !mtab_unusable())
		rc = add_mtab(orig_dev, mountpoint, parsed_info->flags, fstype);

mount_exit:
	if (parsed_info) {
		memset(parsed_info->password, 0, sizeof(parsed_info->password));
		munmap(parsed_info, sizeof(*parsed_info));
	}
	SAFE_FREE(options);
	SAFE_FREE(orgoptions);
	return rc;
}