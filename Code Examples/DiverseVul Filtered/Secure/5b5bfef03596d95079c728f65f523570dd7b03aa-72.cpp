int mainwindow_set_statusbar_lines(MAIN_WINDOW_REC *window,
				   int top, int bottom)
{
	int ret;

        ret = -1;
	if (top != 0) {
                ret = window->statusbar_lines_top;
		window->statusbar_lines_top += top;
                window->statusbar_lines += top;
	}

	if (bottom != 0) {
                ret = window->statusbar_lines_bottom;
                window->statusbar_lines_bottom += bottom;
                window->statusbar_lines += bottom;
	}

	if (top+bottom != 0)
                window->size_dirty = TRUE;

        return ret;
}