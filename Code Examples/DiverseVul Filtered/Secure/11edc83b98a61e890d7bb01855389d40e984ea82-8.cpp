wtap_open_return_val netscreen_open(wtap *wth, int *err, gchar **err_info)
{

	/* Look for a NetScreen snoop header line */
	if (!netscreen_check_file_type(wth, err, err_info)) {
		if (*err != 0 && *err != WTAP_ERR_SHORT_READ)
			return WTAP_OPEN_ERROR;
		return WTAP_OPEN_NOT_MINE;
	}

	if (file_seek(wth->fh, 0L, SEEK_SET, err) == -1)	/* rewind */
		return WTAP_OPEN_ERROR;

	wth->file_encap = WTAP_ENCAP_UNKNOWN;
	wth->file_type_subtype = WTAP_FILE_TYPE_SUBTYPE_NETSCREEN;
	wth->snapshot_length = 0; /* not known */
	wth->subtype_read = netscreen_read;
	wth->subtype_seek_read = netscreen_seek_read;
	wth->file_tsprec = WTAP_TSPREC_DSEC;

	return WTAP_OPEN_MINE;
}