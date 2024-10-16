static int netns_identify(int argc, char **argv)
{
	const char *pidstr;
	char name[256];
	int rc;

	if (argc < 1) {
		pidstr = "self";
	} else if (argc > 1) {
		fprintf(stderr, "extra arguments specified\n");
		return -1;
	} else {
		pidstr = argv[0];
		if (!is_pid(pidstr)) {
			fprintf(stderr, "Specified string '%s' is not a pid\n",
					pidstr);
			return -1;
		}
	}

	rc = netns_identify_pid(pidstr, name, sizeof(name));
	if (!rc)
		printf("%s\n", name);

	return rc;
}