char *lxc_deslashify(const char *path)
{
	char *dup, *p;
	char **parts = NULL;
	size_t n, len;

	dup = strdup(path);
	if (!dup)
		return NULL;

	parts = lxc_normalize_path(dup);
	if (!parts) {
		free(dup);
		return NULL;
	}

	/* We'll end up here if path == "///" or path == "". */
	if (!*parts) {
		len = strlen(dup);
		if (!len) {
			lxc_free_array((void **)parts, free);
			return dup;
		}

		n = strcspn(dup, "/");
		if (n == len) {
			free(dup);
			lxc_free_array((void **)parts, free);

			p = strdup("/");
			if (!p)
				return NULL;

			return p;
		}
	}

	p = lxc_string_join("/", (const char **)parts, *dup == '/');
	free(dup);
	lxc_free_array((void **)parts, free);
	return p;
}