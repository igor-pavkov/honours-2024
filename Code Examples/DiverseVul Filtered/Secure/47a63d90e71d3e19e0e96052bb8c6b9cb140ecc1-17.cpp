void send_xattr_request(const char *fname, struct file_struct *file, int f_out)
{
	const rsync_xa_list *glst = rsync_xal_l.items;
	const item_list *lst;
	int cnt, prior_req = 0;
	rsync_xa *rxa;

	glst += F_XATTR(file);
	lst = &glst->xa_items;

	for (rxa = lst->items, cnt = lst->count; cnt--; rxa++) {
		if (rxa->datum_len <= MAX_FULL_DATUM)
			continue;
		switch (rxa->datum[0]) {
		case XSTATE_ABBREV:
			/* Items left abbreviated matched the sender's checksum, so
			 * the receiver will cache the local data for future use. */
			if (am_generator)
				rxa->datum[0] = XSTATE_DONE;
			continue;
		case XSTATE_TODO:
			assert(f_out >= 0);
			break;
		default:
			continue;
		}

		/* Flag that we handled this abbreviated item. */
		rxa->datum[0] = XSTATE_DONE;

		write_varint(f_out, rxa->num - prior_req);
		prior_req = rxa->num;

		if (fname) {
			size_t len = 0;
			char *ptr;

			/* Re-read the long datum. */
			if (!(ptr = get_xattr_data(fname, rxa->name, &len, 0))) {
				rprintf(FERROR_XFER, "failed to re-read xattr %s for %s\n", rxa->name, fname);
				write_varint(f_out, 0);
				continue;
			}

			write_varint(f_out, len); /* length might have changed! */
			write_bigbuf(f_out, ptr, len);
			free(ptr);
		}
	}

	if (f_out >= 0)
		write_byte(f_out, 0); /* end the list */
}