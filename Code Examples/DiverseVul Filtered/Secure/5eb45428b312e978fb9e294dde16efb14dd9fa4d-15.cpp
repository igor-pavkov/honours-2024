bool detect_ramfs_rootfs(void)
{
	FILE *f;
	char *p, *p2;
	char *line = NULL;
	size_t len = 0;
	int i;

	f = fopen("/proc/self/mountinfo", "r");
	if (!f)
		return false;

	while (getline(&line, &len, f) != -1) {
		for (p = line, i = 0; p && i < 4; i++)
			p = strchr(p + 1, ' ');
		if (!p)
			continue;
		p2 = strchr(p + 1, ' ');
		if (!p2)
			continue;
		*p2 = '\0';
		if (strcmp(p + 1, "/") == 0) {
			// this is '/'.  is it the ramfs?
			p = strchr(p2 + 1, '-');
			if (p && strncmp(p, "- rootfs rootfs ", 16) == 0) {
				free(line);
				fclose(f);
				return true;
			}
		}
	}
	free(line);
	fclose(f);
	return false;
}