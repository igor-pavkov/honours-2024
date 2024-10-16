static inline void phar_unixify_path_separators(char *path, int path_len)
{
	char *s;

	/* unixify win paths */
	for (s = path; s - path < path_len; ++s) {
		if (*s == '\\') {
			*s = '/';
		}
	}
}