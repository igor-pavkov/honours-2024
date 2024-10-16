static int __init cgroup_no_v1(char *str)
{
	struct cgroup_subsys *ss;
	char *token;
	int i;

	while ((token = strsep(&str, ",")) != NULL) {
		if (!*token)
			continue;

		if (!strcmp(token, "all")) {
			cgroup_no_v1_mask = U16_MAX;
			continue;
		}

		if (!strcmp(token, "named")) {
			cgroup_no_v1_named = true;
			continue;
		}

		for_each_subsys(ss, i) {
			if (strcmp(token, ss->name) &&
			    strcmp(token, ss->legacy_name))
				continue;

			cgroup_no_v1_mask |= 1 << i;
		}
	}
	return 1;
}