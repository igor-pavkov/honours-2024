static void mainwindow_resize_windows(MAIN_WINDOW_REC *window)
{
	GSList *tmp;
	int resized;

	mainwindow_set_screen_size(window);

	resized = FALSE;
	for (tmp = windows; tmp != NULL; tmp = tmp->next) {
		WINDOW_REC *rec = tmp->data;

		if (rec->gui_data != NULL &&
		    WINDOW_GUI(rec)->parent == window &&
		    !window_size_equals(rec, window)) {
			resized = TRUE;
			gui_window_resize(rec, MAIN_WINDOW_TEXT_WIDTH(window),
					  MAIN_WINDOW_TEXT_HEIGHT(window));
		}
	}

	if (resized)
		signal_emit("mainwindow resized", 1, window);
}