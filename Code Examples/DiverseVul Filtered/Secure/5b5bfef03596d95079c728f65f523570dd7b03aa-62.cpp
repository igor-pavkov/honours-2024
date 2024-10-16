static int try_rshrink_right(MAIN_WINDOW_REC *window, int count)
{
	MAIN_WINDOW_REC *shrink_win;

	shrink_win = mainwindows_find_right(window, FALSE);
	if (shrink_win != NULL) {
		if (MAIN_WINDOW_TEXT_WIDTH(shrink_win)-count < NEW_WINDOW_WIDTH) {
			return FALSE;
		}

		shrink_win->first_column += count;
		window->last_column += count;

		mainwindows_rresize_two(window, shrink_win, count);
		return TRUE;
	}

	return FALSE;
}