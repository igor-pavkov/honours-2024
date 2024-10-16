static char *langFromHashbang(RCore *core, const char *file) {
	int fd = r_sandbox_open (file, O_RDONLY, 0);
	if (fd != -1) {
		char firstLine[128] = {0};
		int len = r_sandbox_read (fd, (ut8*)firstLine, sizeof (firstLine) - 1);
		firstLine[len] = 0;
		if (!strncmp (firstLine, "#!/", 3)) {
			// I CAN HAS A HASHBANG
			char *nl = strchr (firstLine, '\n');
			if (nl) {
				*nl = 0;
			}
			nl = strchr (firstLine, ' ');
			if (nl) {
				*nl = 0;
			}
			return strdup (firstLine + 2);
		}
		r_sandbox_close (fd);
	}
	return NULL;
}