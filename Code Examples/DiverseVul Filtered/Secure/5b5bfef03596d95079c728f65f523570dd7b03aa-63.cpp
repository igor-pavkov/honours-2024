static MAIN_WINDOW_REC *mainwindows_find_lower_right(MAIN_WINDOW_REC *window)
{
	MAIN_WINDOW_REC *best;

	best = mainwindows_find_right(window, FALSE);
	if (best == NULL)
		best = mainwindows_find_lower(window);

	return best;
}