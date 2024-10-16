static int mainwindow_rgrow(MAIN_WINDOW_REC *window, int count)
{
	if (!try_rshrink_right(window, count)) {
		if (!try_rshrink_left(window, count))
			return FALSE;
	}

	return TRUE;
}