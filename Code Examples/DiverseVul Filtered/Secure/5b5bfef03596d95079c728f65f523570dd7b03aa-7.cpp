static GSList *get_sticky_windows_sorted(MAIN_WINDOW_REC *mainwin)
{
	GSList *tmp, *list;

        list = NULL;
	for (tmp = windows; tmp != NULL; tmp = tmp->next) {
		WINDOW_REC *rec = tmp->data;

		if (WINDOW_GUI(rec)->sticky && WINDOW_MAIN(rec) == mainwin) {
			list = g_slist_insert_sorted(list, rec, (GCompareFunc)
						     window_refnum_cmp);
		}
	}

        return list;
}