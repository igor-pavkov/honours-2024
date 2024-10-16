static bool may_access_netns(int pid)
{
	int ret;
	char s[200];
	uid_t ruid, suid, euid;
	bool may_access = false;

	ret = getresuid(&ruid, &euid, &suid);
	if (ret < 0) {
		usernic_error("Failed to retrieve real, effective, and saved "
			      "user IDs: %s\n",
			      strerror(errno));
		return false;
	}

	ret = setresuid(ruid, ruid, euid);
	if (ret < 0) {
		usernic_error("Failed to drop privilege by setting effective "
			      "user id and real user id to %d, and saved user "
			      "ID to %d: %s\n",
			      ruid, euid, strerror(errno));
		return false;
	}

	ret = snprintf(s, 200, "/proc/%d/ns/net", pid);
	if (ret < 0 || ret >= 200)
		return false;

	ret = access(s, R_OK);
	may_access = true;
	if (ret < 0)  {
		may_access = false;
		usernic_error("Uid %d may not access %s: %s\n", (int)ruid, s, strerror(errno));
	}

	ret = setresuid(ruid, euid, suid);
	if (ret < 0) {
		usernic_error("Failed to restore user id to %d, real user id "
			      "to %d, and saved user ID to %d: %s\n",
			      ruid, euid, suid, strerror(errno));
		may_access = false;
	}

	return may_access;
}