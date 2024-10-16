void mainwindows_recreate(void)
{
	GSList *tmp;

	for (tmp = mainwindows; tmp != NULL; tmp = tmp->next) {
		MAIN_WINDOW_REC *rec = tmp->data;

		rec->screen_win = mainwindow_create_screen(rec);
                rec->dirty = TRUE;
		textbuffer_view_set_window(WINDOW_GUI(rec->active)->view,
					   rec->screen_win);
	}
}