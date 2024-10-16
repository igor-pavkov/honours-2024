void mainwindow_destroy(MAIN_WINDOW_REC *window)
{
	g_return_if_fail(window != NULL);

	mainwindows = g_slist_remove(mainwindows, window);
	signal_emit("mainwindow destroyed", 1, window);

        term_window_destroy(window->screen_win);

	if (mainwindows != NULL) {
		gui_windows_remove_parent(window);
		if (!quitting) {
			mainwindows_add_space(window);
			mainwindows_redraw();
		}
	}

	g_free(window);

	if (active_mainwin == window) active_mainwin = NULL;
}