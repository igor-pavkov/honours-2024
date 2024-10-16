static gboolean netscreen_read(wtap *wth, int *err, gchar **err_info,
    gint64 *data_offset)
{
	gint64		offset;
	char		line[NETSCREEN_LINE_LENGTH];

	/* Find the next packet */
	offset = netscreen_seek_next_packet(wth, err, err_info, line);
	if (offset < 0)
		return FALSE;

	/* Parse the header and convert the ASCII hex dump to binary data */
	if (!parse_netscreen_packet(wth->fh, &wth->phdr,
	    wth->frame_buffer, line, err, err_info))
		return FALSE;

	/*
	 * If the per-file encapsulation isn't known, set it to this
	 * packet's encapsulation.
	 *
	 * If it *is* known, and it isn't this packet's encapsulation,
	 * set it to WTAP_ENCAP_PER_PACKET, as this file doesn't
	 * have a single encapsulation for all packets in the file.
	 */
	if (wth->file_encap == WTAP_ENCAP_UNKNOWN)
		wth->file_encap = wth->phdr.pkt_encap;
	else {
		if (wth->file_encap != wth->phdr.pkt_encap)
			wth->file_encap = WTAP_ENCAP_PER_PACKET;
	}

	*data_offset = offset;
	return TRUE;
}