static int calc_shared_perm(int mode)
{
	int tweak;

	if (get_shared_repository() < 0)
		tweak = -get_shared_repository();
	else
		tweak = get_shared_repository();

	if (!(mode & S_IWUSR))
		tweak &= ~0222;
	if (mode & S_IXUSR)
		/* Copy read bits to execute bits */
		tweak |= (tweak & 0444) >> 2;
	if (get_shared_repository() < 0)
		mode = (mode & ~0777) | tweak;
	else
		mode |= tweak;

	return mode;
}