static bool is_privileged_over_netns(int netns_fd)
{
	int ret;
	pid_t pid_self;
	uid_t euid, ruid, suid;
	bool bret = false;
	int ofd = -1;

	pid_self = lxc_raw_getpid();
	ofd = lxc_preserve_ns(pid_self, "net");
	if (ofd < 0) {
		usernic_error("Failed opening network namespace path for %d", pid_self);
		return false;
	}

	ret = getresuid(&ruid, &euid, &suid);
	if (ret < 0) {
		usernic_error("Failed to retrieve real, effective, and saved "
			      "user IDs: %s\n",
			      strerror(errno));
		goto do_partial_cleanup;
	}

	ret = setns(netns_fd, CLONE_NEWNET);
	if (ret < 0) {
		usernic_error("Failed to setns() to network namespace %s\n",
			      strerror(errno));
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

	/* Test whether we are privileged over the network namespace. To do this
	 * we try to delete the loopback interface which is not possible. If we
	 * are privileged over the network namespace we will get ENOTSUP. If we
	 * are not privileged over the network namespace we will get EPERM.
	 */
	ret = lxc_netdev_delete_by_name("lo");
	if (ret == -ENOTSUP)
		bret = true;

do_full_cleanup:
	ret = setresuid(ruid, euid, suid);
	if (ret < 0) {
		usernic_error("Failed to restore privilege by setting "
			      "effective user id to %d, real user id to %d, "
			      "and saved user ID to %d: %s\n", ruid, euid, suid,
			      strerror(errno));

		bret = false;
	}

	ret = setns(ofd, CLONE_NEWNET);
	if (ret < 0) {
		usernic_error("Failed to setns() to original network namespace "
			      "of PID %d: %s\n", ofd, strerror(errno));

		bret = false;
	}

do_partial_cleanup:

	close(ofd);
	return bret;
}