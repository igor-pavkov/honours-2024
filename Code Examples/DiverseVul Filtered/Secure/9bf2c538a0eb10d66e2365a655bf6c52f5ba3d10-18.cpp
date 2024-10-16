static int on_netns_del(char *nsname, void *arg)
{
	char netns_path[PATH_MAX];

	snprintf(netns_path, sizeof(netns_path), "%s/%s", NETNS_RUN_DIR, nsname);
	umount2(netns_path, MNT_DETACH);
	if (unlink(netns_path) < 0) {
		fprintf(stderr, "Cannot remove namespace file \"%s\": %s\n",
			netns_path, strerror(errno));
		return -1;
	}
	return 0;
}