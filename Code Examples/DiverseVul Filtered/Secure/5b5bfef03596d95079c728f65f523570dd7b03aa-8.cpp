static void window_reparent(WINDOW_REC *win, MAIN_WINDOW_REC *mainwin)
{
	MAIN_WINDOW_REC *old_mainwin;

	old_mainwin = WINDOW_MAIN(win);

	if (old_mainwin != mainwin) {
		gui_window_set_unsticky(win);

		if (old_mainwin->active == win) {
			mainwindow_change_active(old_mainwin, win);
			if (active_mainwin == NULL) {
				active_mainwin = mainwin;
				window_set_active(mainwin->active);
			}
		}

		gui_window_reparent(win, mainwin);
		window_set_active(win);
	}
}