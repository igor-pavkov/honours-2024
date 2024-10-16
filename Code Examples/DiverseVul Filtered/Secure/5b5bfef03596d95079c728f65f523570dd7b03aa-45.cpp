static void mainwindows_resize_smaller(int ydiff)
{
	MAIN_WINDOW_REC *rec;
	GSList *sorted, *tmp;
	int space;

	sorted = NULL;
	for (rec = mainwindows_find_lower(NULL);
	     rec != NULL;
	     rec = mainwindows_find_lower(rec)) {
		sorted = g_slist_prepend(sorted, rec);
	}
	if (sorted == NULL)
		return;

	for (;;) {
		int skip_active = FALSE;
		space = 0;
		/* for each line of windows, calculate the space that can be reduced still */
		for (tmp = sorted; tmp != NULL; tmp = tmp->next) {
			int min;
			GSList *line, *ltmp;
			rec = tmp->data;
			line = mainwindows_get_line(rec);
			min = screen_height - ydiff;
			for (ltmp = line; ltmp != NULL; ltmp = ltmp->next) {
				int lmin;
				MAIN_WINDOW_REC *win = ltmp->data;
				if (win == active_mainwin && tmp == sorted)
					skip_active = TRUE;

				lmin = MAIN_WINDOW_TEXT_HEIGHT(win)-WINDOW_MIN_SIZE;
				if (lmin < min)
					min = lmin;
			}
			g_slist_free(line);
			space += min;
		}

		if (space >= -ydiff)
			break;

		rec = sorted->data;
		if (skip_active && sorted->next != NULL)
			rec = sorted->next->data;
		sorted = g_slist_remove(sorted, rec);

		if (sorted != NULL) {
			/* terminal is too small - destroy the
			   uppest window and try again */
			GSList *line, *ltmp;
			line = mainwindows_get_line(rec);
			for (ltmp = line; ltmp != NULL; ltmp = ltmp->next) {
				MAIN_WINDOW_REC *win = ltmp->data;
				mainwindow_destroy(win);
			}
			g_slist_free(line);
		} else {
			/* only one line of window in screen.. just force the resize */
			GSList *line, *ltmp;
			line = mainwindows_get_line(rec);
			for (ltmp = line; ltmp != NULL; ltmp = ltmp->next) {
				MAIN_WINDOW_REC *win = ltmp->data;
				win->last_line += ydiff;
				mainwindow_resize(win, 0, ydiff);
			}
			g_slist_free(line);
			return;
		}
	}

	/* resize windows that have space */
	for (tmp = sorted; tmp != NULL && ydiff < 0; tmp = tmp->next) {
		int min;
		GSList *line, *ltmp;

		rec = tmp->data;
		line = mainwindows_get_line(rec);
		min = screen_height - ydiff;
		for (ltmp = line; ltmp != NULL; ltmp = ltmp->next) {
			int lmin;
			MAIN_WINDOW_REC *win = ltmp->data;
			lmin = MAIN_WINDOW_TEXT_HEIGHT(win)-WINDOW_MIN_SIZE;
			if (lmin < min)
				min = lmin;
		}
		space = min;

		if (space == 0) {
			/* move the line */
			for (ltmp = line; ltmp != NULL; ltmp = ltmp->next) {
				MAIN_WINDOW_REC *win = ltmp->data;
				mainwindow_resize(win, 0, 0);
				win->size_dirty = TRUE;
				win->first_line += ydiff;
				win->last_line += ydiff;
				signal_emit("mainwindow moved", 1, win);
			}
		} else {
			if (space > -ydiff) space = -ydiff;
			for (ltmp = line; ltmp != NULL; ltmp = ltmp->next) {
				MAIN_WINDOW_REC *win = ltmp->data;
				win->last_line += ydiff;
				win->first_line += ydiff + space;

				mainwindow_resize(win, 0, -space);
			}
			ydiff += space;
		}
		g_slist_free(line);
	}

	g_slist_free(sorted);
}