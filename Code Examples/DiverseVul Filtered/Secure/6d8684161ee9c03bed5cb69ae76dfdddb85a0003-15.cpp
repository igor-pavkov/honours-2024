static void setup_windows_environment(void)
{
	char *tmp = getenv("TMPDIR");

	/* on Windows it is TMP and TEMP */
	if (!tmp) {
		if (!(tmp = getenv("TMP")))
			tmp = getenv("TEMP");
		if (tmp) {
			setenv("TMPDIR", tmp, 1);
			tmp = getenv("TMPDIR");
		}
	}

	if (tmp) {
		/*
		 * Convert all dir separators to forward slashes,
		 * to help shell commands called from the Git
		 * executable (by not mistaking the dir separators
		 * for escape characters).
		 */
		convert_slashes(tmp);
	}

	/* simulate TERM to enable auto-color (see color.c) */
	if (!getenv("TERM"))
		setenv("TERM", "cygwin", 1);
}