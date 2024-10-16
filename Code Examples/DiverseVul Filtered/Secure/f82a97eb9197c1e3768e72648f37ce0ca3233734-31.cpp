int mingw_has_dos_drive_prefix(const char *path)
{
	int i;

	/*
	 * Does it start with an ASCII letter (i.e. highest bit not set),
	 * followed by a colon?
	 */
	if (!(0x80 & (unsigned char)*path))
		return *path && path[1] == ':' ? 2 : 0;

	/*
	 * While drive letters must be letters of the English alphabet, it is
	 * possible to assign virtually _any_ Unicode character via `subst` as
	 * a drive letter to "virtual drives". Even `1`, or `ä`. Or fun stuff
	 * like this:
	 *
	 *      subst ֍: %USERPROFILE%\Desktop
	 */
	for (i = 1; i < 4 && (0x80 & (unsigned char)path[i]); i++)
		; /* skip first UTF-8 character */
	return path[i] == ':' ? i + 1 : 0;
}