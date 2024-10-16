static MAIN_WINDOW_REC *mainwindows_find_upper_left(MAIN_WINDOW_REC *window)
{
	MAIN_WINDOW_REC *best;

	best = mainwindows_find_left(window, FALSE);
	if (best == NULL)
		best = mainwindows_find_upper(window);

	return best;
}