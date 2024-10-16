static int mainwindow_grow(MAIN_WINDOW_REC *window, int count,
			   int resize_lower)
{
	if (!resize_lower || !try_shrink_lower(window, count)) {
		if (!try_shrink_upper(window, count)) {
                        if (resize_lower || !try_shrink_lower(window, count))
				return FALSE;
		}
	}

        return TRUE;
}