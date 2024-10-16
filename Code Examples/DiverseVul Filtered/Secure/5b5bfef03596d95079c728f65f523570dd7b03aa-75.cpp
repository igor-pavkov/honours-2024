static void gui_windows_remove_parent(MAIN_WINDOW_REC *window)
{
        MAIN_WINDOW_REC *new_parent;
	GSList *tmp;

        new_parent = mainwindows->data;
	for (tmp = windows; tmp != NULL; tmp = tmp->next) {
		WINDOW_REC *rec = tmp->data;

		if (rec->gui_data != NULL && WINDOW_MAIN(rec) == window)
                        gui_window_reparent(rec, new_parent);
	}
}