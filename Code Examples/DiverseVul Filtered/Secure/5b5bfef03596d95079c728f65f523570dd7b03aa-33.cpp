static int try_rshrink_left(MAIN_WINDOW_REC *window, int count)
{
	MAIN_WINDOW_REC *shrink_win;

	shrink_win = mainwindows_find_left(window, FALSE);
	if (shrink_win != NULL) {
		if (MAIN_WINDOW_TEXT_WIDTH(shrink_win)-count < NEW_WINDOW_WIDTH) {
			return FALSE;
		}
		window->first_column -= count;
		shrink_win->last_column -= count;

		mainwindows_rresize_two(window, shrink_win, count);
		return TRUE;
	}

	return FALSE;
}