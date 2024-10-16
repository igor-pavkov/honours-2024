static int mainwindow_rshrink(MAIN_WINDOW_REC *window, int count)
{
	if (MAIN_WINDOW_TEXT_WIDTH(window)-count < NEW_WINDOW_WIDTH)
		return FALSE;

	if (!try_rgrow_right(window, count)) {
		if (!try_rgrow_left(window, count))
			return FALSE;
	}

	return TRUE;
}