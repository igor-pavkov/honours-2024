static MAIN_WINDOW_REC *mainwindows_find_left_upper(MAIN_WINDOW_REC *window)
{
	MAIN_WINDOW_REC *best;

	best = mainwindows_find_left(window, FALSE);
	if (best == NULL)
		best = mainwindows_find_left(mainwindows_find_upper(window), TRUE);

	return best;
}