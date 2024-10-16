static int mainwindow_shrink(MAIN_WINDOW_REC *window, int count, int resize_lower)
{
	if (MAIN_WINDOW_TEXT_HEIGHT(window)-count < WINDOW_MIN_SIZE)
                return FALSE;

	if (!resize_lower || !try_grow_lower(window, count)) {
		if (!try_grow_upper(window, count)) {
                        if (resize_lower || !try_grow_lower(window, count))
				return FALSE;
		}
	}

        return TRUE;
}