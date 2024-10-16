static inline int needs_hiding(const char *path)
{
	const char *basename;

	if (hide_dotfiles == HIDE_DOTFILES_FALSE)
		return 0;

	/* We cannot use basename(), as it would remove trailing slashes */
	mingw_skip_dos_drive_prefix((char **)&path);
	if (!*path)
		return 0;

	for (basename = path; *path; path++)
		if (is_dir_sep(*path)) {
			do {
				path++;
			} while (is_dir_sep(*path));
			/* ignore trailing slashes */
			if (*path)
				basename = path;
		}

	if (hide_dotfiles == HIDE_DOTFILES_TRUE)
		return *basename == '.';

	assert(hide_dotfiles == HIDE_DOTFILES_DOTGITONLY);
	return !strncasecmp(".git", basename, 4) &&
		(!basename[4] || is_dir_sep(basename[4]));
}