MAIN_WINDOW_REC *mainwindow_create(int right)
{
	MAIN_WINDOW_REC *rec, *parent;
	int space;

	rec = g_new0(MAIN_WINDOW_REC, 1);
	rec->dirty = TRUE;

	if (mainwindows == NULL) {
		active_mainwin = rec;

		rec->first_line = screen_reserved_top;
		rec->last_line = term_height-1 - screen_reserved_bottom;
		rec->height = rec->last_line-rec->first_line+1;
		rec->first_column = screen_reserved_left;
		rec->last_column = screen_width-1 - screen_reserved_right;
		rec->width = rec->last_column-rec->first_column+1;
	} else {
		parent = WINDOW_MAIN(active_win);

		if (!right) {
			GSList *tmp, *line;
			if (MAIN_WINDOW_TEXT_HEIGHT(parent) <
			    WINDOW_MIN_SIZE+NEW_WINDOW_SIZE)
				parent = find_window_with_room();
			if (parent == NULL)
				return NULL; /* not enough space */

			space = parent->height / 2;
			rec->first_line = parent->first_line;
			rec->last_line = rec->first_line + space;
			rec->height = rec->last_line-rec->first_line+1;
			rec->first_column = screen_reserved_left;
			rec->last_column = screen_width-1 - screen_reserved_right;
			rec->width = rec->last_column-rec->first_column+1;

			line = mainwindows_get_line(parent);
			for (tmp = line; tmp != NULL; tmp = tmp->next) {
				MAIN_WINDOW_REC *rec = tmp->data;
				rec->first_line += space+1;
				mainwindow_resize(rec, 0, -space-1);
			}
			g_slist_free(line);
		} else {
			if (MAIN_WINDOW_TEXT_WIDTH(parent) <
			    2* NEW_WINDOW_WIDTH)
				parent = find_window_with_room_right();
			if (parent == NULL)
				return NULL; /* not enough space */

			space = parent->width / 2;
			rec->first_line = parent->first_line;
			rec->last_line = parent->last_line;
			rec->height = parent->height;
			rec->first_column = parent->last_column - space + 1;
			rec->last_column = parent->last_column;
			rec->width = rec->last_column-rec->first_column+1;

			parent->last_column -= space+1;
			mainwindow_resize(parent, -space-1, 0);
		}
	}

	rec->screen_win = mainwindow_create_screen(rec);
	term_refresh(NULL);

	mainwindows = g_slist_append(mainwindows, rec);
	signal_emit("mainwindow created", 1, rec);
	return rec;
}