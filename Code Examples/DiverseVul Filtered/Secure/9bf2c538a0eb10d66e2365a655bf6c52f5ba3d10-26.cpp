static int netns_add(int argc, char **argv, bool create)
{
	/* This function creates a new network namespace and
	 * a new mount namespace and bind them into a well known
	 * location in the filesystem based on the name provided.
	 *
	 * If create is true, a new namespace will be created,
	 * otherwise an existing one will be attached to the file.
	 *
	 * The mount namespace is created so that any necessary
	 * userspace tweaks like remounting /sys, or bind mounting
	 * a new /etc/resolv.conf can be shared between users.
	 */
	char netns_path[PATH_MAX], proc_path[PATH_MAX];
	const char *name;
	pid_t pid;
	int fd;
	int made_netns_run_dir_mount = 0;

	if (create) {
		if (argc < 1) {
			fprintf(stderr, "No netns name specified\n");
			return -1;
		}
	} else {
		if (argc < 2) {
			fprintf(stderr, "No netns name and PID specified\n");
			return -1;
		}

		if (get_s32(&pid, argv[1], 0) || !pid) {
			fprintf(stderr, "Invalid PID: %s\n", argv[1]);
			return -1;
		}
	}
	name = argv[0];

	snprintf(netns_path, sizeof(netns_path), "%s/%s", NETNS_RUN_DIR, name);

	if (create_netns_dir())
		return -1;

	/* Make it possible for network namespace mounts to propagate between
	 * mount namespaces.  This makes it likely that a unmounting a network
	 * namespace file in one namespace will unmount the network namespace
	 * file in all namespaces allowing the network namespace to be freed
	 * sooner.
	 */
	while (mount("", NETNS_RUN_DIR, "none", MS_SHARED | MS_REC, NULL)) {
		/* Fail unless we need to make the mount point */
		if (errno != EINVAL || made_netns_run_dir_mount) {
			fprintf(stderr, "mount --make-shared %s failed: %s\n",
				NETNS_RUN_DIR, strerror(errno));
			return -1;
		}

		/* Upgrade NETNS_RUN_DIR to a mount point */
		if (mount(NETNS_RUN_DIR, NETNS_RUN_DIR, "none", MS_BIND | MS_REC, NULL)) {
			fprintf(stderr, "mount --bind %s %s failed: %s\n",
				NETNS_RUN_DIR, NETNS_RUN_DIR, strerror(errno));
			return -1;
		}
		made_netns_run_dir_mount = 1;
	}

	/* Create the filesystem state */
	fd = open(netns_path, O_RDONLY|O_CREAT|O_EXCL, 0);
	if (fd < 0) {
		fprintf(stderr, "Cannot create namespace file \"%s\": %s\n",
			netns_path, strerror(errno));
		return -1;
	}
	close(fd);

	if (create) {
		if (unshare(CLONE_NEWNET) < 0) {
			fprintf(stderr, "Failed to create a new network namespace \"%s\": %s\n",
				name, strerror(errno));
			goto out_delete;
		}

		strcpy(proc_path, "/proc/self/ns/net");
	} else {
		snprintf(proc_path, sizeof(proc_path), "/proc/%d/ns/net", pid);
	}

	/* Bind the netns last so I can watch for it */
	if (mount(proc_path, netns_path, "none", MS_BIND, NULL) < 0) {
		fprintf(stderr, "Bind %s -> %s failed: %s\n",
			proc_path, netns_path, strerror(errno));
		goto out_delete;
	}
	return 0;
out_delete:
	if (create) {
		netns_delete(argc, argv);
	} else if (unlink(netns_path) < 0) {
		fprintf(stderr, "Cannot remove namespace file \"%s\": %s\n",
			netns_path, strerror(errno));
	}
	return -1;
}