static void show_no_layers_warning (void) {
	gchar *str = g_new(gchar, MAX_DISTLEN);
	utf8_strncpy(str, _("No layers are currently loaded. A layer must be loaded first."), MAX_DISTLEN - 7);
	utf8_snprintf(screen.statusbar.diststr, MAX_DISTLEN, "<b>%s</b>", str);
	g_free(str);
		
	callbacks_update_statusbar();
}