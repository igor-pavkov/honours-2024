static char *lxc_secure_rename_in_ns(int pid, char *oldname, char *newname,
				     int *container_veth_ifidx)
{
	int ret;
	pid_t pid_self;
	uid_t ruid, suid, euid;
	char ifname[IFNAMSIZ];
	char *string_ret = NULL, *name = NULL;
	int fd = -1, ifindex = -1, ofd = -1;

	pid_self = lxc_raw_getpid();
	ofd = lxc_preserve_ns(pid_self, "net");
	if (ofd < 0) {
		usernic_error("Failed opening network namespace path for %d", pid_self);
		return NULL;
	}

	fd = lxc_preserve_ns(pid, "net");
	if (fd < 0) {
		usernic_error("Failed opening network namespace path for %d", pid);
		goto do_partial_cleanup;
	}

	ret = getresuid(&ruid, &euid, &suid);
	if (ret < 0) {
		usernic_error("Failed to retrieve real, effective, and saved "
			      "user IDs: %s\n",
			      strerror(errno));
		goto do_partial_cleanup;
	}

	ret = setns(fd, CLONE_NEWNET);
	close(fd);
	fd = -1;
	if (ret < 0) {
		usernic_error("Failed to setns() to the network namespace of "
			      "the container with PID %d: %s\n",
			      pid, strerror(errno));
		goto do_partial_cleanup;
	}

	ret = setresuid(ruid, ruid, 0);
	if (ret < 0) {
		usernic_error("Failed to drop privilege by setting effective "
			      "user id and real user id to %d, and saved user "
			      "ID to 0: %s\n",
			      ruid, strerror(errno));
		/* It's ok to jump to do_full_cleanup here since setresuid()
		 * will succeed when trying to set real, effective, and saved to
		 * values they currently have.
		 */
		goto do_full_cleanup;
	}

	/* Check if old interface exists. */
	ifindex = if_nametoindex(oldname);
	if (!ifindex) {
		usernic_error("Failed to get netdev index: %s\n", strerror(errno));
		goto do_full_cleanup;
	}

	/* When the IFLA_IFNAME attribute is passed something like "<prefix>%d"
	 * netlink will replace the format specifier with an appropriate index.
	 * So we pass "eth%d".
	 */
	if (newname)
		name = newname;
	else
		name = "eth%d";

	ret = lxc_netdev_rename_by_name(oldname, name);
	name = NULL;
	if (ret < 0) {
		usernic_error("Error %d renaming netdev %s to %s in container\n",
			      ret, oldname, newname ? newname : "eth%d");
		goto do_full_cleanup;
	}

	/* Retrieve new name for interface. */
	if (!if_indextoname(ifindex, ifname)) {
		usernic_error("Failed to get new netdev name: %s\n", strerror(errno));
		goto do_full_cleanup;
	}

	/* Allocation failure for strdup() is checked below. */
	name = strdup(ifname);
	string_ret = name;
	*container_veth_ifidx = ifindex;

do_full_cleanup:
	ret = setresuid(ruid, euid, suid);
	if (ret < 0) {
		usernic_error("Failed to restore privilege by setting "
			      "effective user id to %d, real user id to %d, "
			      "and saved user ID to %d: %s\n", ruid, euid, suid,
			      strerror(errno));

		string_ret = NULL;
	}

	ret = setns(ofd, CLONE_NEWNET);
	if (ret < 0) {
		usernic_error("Failed to setns() to original network namespace "
			      "of PID %d: %s\n", ofd, strerror(errno));

		string_ret = NULL;
	}

do_partial_cleanup:
	if (fd >= 0)
		close(fd);

	if (!string_ret && name)
		free(name);

	close(ofd);

	return string_ret;
}