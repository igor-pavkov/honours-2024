GSList *mainwindows_get_sorted(int reverse)
{
	GSList *tmp, *list;

	list = NULL;
	for (tmp = mainwindows; tmp != NULL; tmp = tmp->next) {
		list = g_slist_insert_sorted(list, tmp->data, (GCompareFunc)
					     (reverse ? mainwindows_compare_reverse : mainwindows_compare));
	}

	return list;
}