void mainwindow_change_active(MAIN_WINDOW_REC *mainwin,
			      WINDOW_REC *skip_window)
{
        WINDOW_REC *window, *other;
	GSList *tmp;

        mainwin->active = NULL;
	if (mainwin->sticky_windows) {
		/* sticky window */
                tmp = get_sticky_windows_sorted(mainwin);
                window = tmp->data;
		if (window == skip_window) {
			window = tmp->next == NULL ? NULL :
				tmp->next->data;
		}
                g_slist_free(tmp);

		if (window != NULL) {
			window_set_active(window);
			return;
		}
	}

        other = NULL;
	for (tmp = windows; tmp != NULL; tmp = tmp->next) {
		WINDOW_REC *rec = tmp->data;

		if (rec != skip_window) {
			other = rec;
			break;
		}
	}

	window_set_active(other);
	if (mainwindows->next != NULL)
		mainwindow_destroy(mainwin);
}