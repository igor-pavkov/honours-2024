void mainwindows_redraw(void)
{
        GSList *tmp;

        irssi_set_dirty();
	for (tmp = mainwindows; tmp != NULL; tmp = tmp->next) {
		MAIN_WINDOW_REC *rec = tmp->data;

                rec->dirty = TRUE;
	}
}