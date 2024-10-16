int adjust_shared_perm(const char *path)
{
	int old_mode, new_mode;

	if (!get_shared_repository())
		return 0;
	if (get_st_mode_bits(path, &old_mode) < 0)
		return -1;

	new_mode = calc_shared_perm(old_mode);
	if (S_ISDIR(old_mode)) {
		/* Copy read bits to execute bits */
		new_mode |= (new_mode & 0444) >> 2;
		new_mode |= FORCE_DIR_SET_GID;
	}

	if (((old_mode ^ new_mode) & ~S_IFMT) &&
			chmod(path, (new_mode & ~S_IFMT)) < 0)
		return -2;
	return 0;
}