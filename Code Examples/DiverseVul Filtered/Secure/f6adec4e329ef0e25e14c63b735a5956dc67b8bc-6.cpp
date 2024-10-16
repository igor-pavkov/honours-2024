int check_submodule_name(const char *name)
{
	/* Disallow empty names */
	if (!*name)
		return -1;

	/*
	 * Look for '..' as a path component. Check both '/' and '\\' as
	 * separators rather than is_dir_sep(), because we want the name rules
	 * to be consistent across platforms.
	 */
	goto in_component; /* always start inside component */
	while (*name) {
		char c = *name++;
		if (c == '/' || c == '\\') {
in_component:
			if (name[0] == '.' && name[1] == '.' &&
			    (!name[2] || name[2] == '/' || name[2] == '\\'))
				return -1;
		}
	}

	return 0;
}