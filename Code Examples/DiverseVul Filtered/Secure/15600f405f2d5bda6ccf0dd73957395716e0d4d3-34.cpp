log_create_pathname (char *servname, char *channame, char *netname)
{
	char fname[384];
	char fnametime[384];
	time_t now;

	if (!netname)
	{
		netname = g_strdup ("NETWORK");
	}
	else
	{
		netname = log_create_filename (netname);
	}

	/* first, everything is in UTF-8 */
	if (!rfc_casecmp (channame, servname))
	{
		channame = g_strdup ("server");
	}
	else
	{
		channame = log_create_filename (channame);
	}

	servname = log_create_filename (servname);

	log_insert_vars (fname, sizeof (fname), prefs.hex_irc_logmask, channame, netname, servname);
	g_free (channame);
	g_free (netname);
	g_free (servname);

	/* insert time/date */
	now = time (NULL);
	strftime_utf8 (fnametime, sizeof (fnametime), fname, now);

	/* If one uses log mask variables, such as "%c/...", %c will be empty upon
	 * connecting since there's no channel name yet, so we have to make sure
	 * we won't try to write to the FS root. */
	if (g_path_is_absolute (prefs.hex_irc_logmask))
	{
		g_snprintf (fname, sizeof (fname), "%s", fnametime);
	}
	else	/* relative path */
	{
		g_snprintf (fname, sizeof (fname), "%s" G_DIR_SEPARATOR_S "logs" G_DIR_SEPARATOR_S "%s", get_xdir (), fnametime);
	}

	/* create all the subdirectories */
	mkdir_p (fname);

	return g_strdup (fname);
}