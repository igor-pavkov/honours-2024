scrollback_save (session *sess, char *text, time_t stamp)
{
	GOutputStream *ostream;
	char *buf;

	if (sess->type == SESS_SERVER && prefs.hex_gui_tab_server == 1)
		return;

	if (sess->text_scrollback == SET_DEFAULT)
	{
		if (!prefs.hex_text_replay)
			return;
	}
	else
	{
		if (sess->text_scrollback != SET_ON)
			return;
	}

	if (!sess->scrollfile)
	{
		if ((buf = scrollback_get_filename (sess)) == NULL)
			return;

		sess->scrollfile = g_file_new_for_path (buf);
		g_free (buf);
	}
	else
	{
		/* Users can delete the folder after it's created... */
		GFile *parent = g_file_get_parent (sess->scrollfile);
		g_file_make_directory_with_parents (parent, NULL, NULL);
		g_object_unref (parent);
	}

	ostream = G_OUTPUT_STREAM(g_file_append_to (sess->scrollfile, G_FILE_CREATE_PRIVATE, NULL, NULL));
	if (!ostream)
		return;

	if (!stamp)
		stamp = time(0);
	if (sizeof (stamp) == 4)	/* gcc will optimize one of these out */
		buf = g_strdup_printf ("T %d ", (int) stamp);
	else
		buf = g_strdup_printf ("T %" G_GINT64_FORMAT " ", (gint64)stamp);

	g_output_stream_write (ostream, buf, strlen (buf), NULL, NULL);
	g_output_stream_write (ostream, text, strlen (text), NULL, NULL);
	if (!g_str_has_suffix (text, "\n"))
		g_output_stream_write (ostream, "\n", 1, NULL, NULL);

	g_free (buf);
	g_object_unref (ostream);

	sess->scrollwritten++;

	if ((sess->scrollwritten > prefs.hex_text_max_lines && prefs.hex_text_max_lines > 0) ||
       sess->scrollwritten > SCROLLBACK_MAX)
		scrollback_shrink (sess);
}