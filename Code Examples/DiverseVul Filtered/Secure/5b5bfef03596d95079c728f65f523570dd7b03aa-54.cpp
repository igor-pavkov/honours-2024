static void windows_print_sticky(WINDOW_REC *win)
{
        MAIN_WINDOW_REC *mainwin;
        GSList *tmp, *list;
	GString *str;

        mainwin = WINDOW_MAIN(win);

        /* convert to string */
	str = g_string_new(NULL);
	list = get_sticky_windows_sorted(mainwin);
	for (tmp = list; tmp != NULL; tmp = tmp->next) {
		WINDOW_REC *rec = tmp->data;

		g_string_append_printf(str, "#%d, ", rec->refnum);
	}
        g_string_truncate(str, str->len-2);
        g_slist_free(list);

	printformat_window(win, MSGLEVEL_CLIENTCRAP,
			   TXT_WINDOW_INFO_STICKY, str->str);
        g_string_free(str, TRUE);
}