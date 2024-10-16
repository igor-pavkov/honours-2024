void mainwindows_redraw_dirty(void)
{
	GSList *tmp;

	for (tmp = mainwindows; tmp != NULL; tmp = tmp->next) {
		MAIN_WINDOW_REC *rec = tmp->data;

		if (rec->size_dirty) {
                        rec->size_dirty = FALSE;
			mainwindow_resize_windows(rec);
		}
		if (rec->dirty) {
                        rec->dirty = FALSE;
			gui_window_redraw(rec->active);
		} else if (WINDOW_GUI(rec->active)->view->dirty) {
			gui_window_redraw(rec->active);
		}
	}
}