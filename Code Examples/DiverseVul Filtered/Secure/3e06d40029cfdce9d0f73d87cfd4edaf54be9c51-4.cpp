static void handle_delayed_updates(char *local_name)
{
	char *fname, *partialptr;
	int ndx;

	for (ndx = -1; (ndx = bitbag_next_bit(delayed_bits, ndx)) >= 0; ) {
		struct file_struct *file = cur_flist->files[ndx];
		fname = local_name ? local_name : f_name(file, NULL);
		if ((partialptr = partial_dir_fname(fname)) != NULL) {
			if (make_backups > 0 && !make_backup(fname, False))
				continue;
			if (DEBUG_GTE(RECV, 1)) {
				rprintf(FINFO, "renaming %s to %s\n",
					partialptr, fname);
			}
			/* We don't use robust_rename() here because the
			 * partial-dir must be on the same drive. */
			if (do_rename(partialptr, fname) < 0) {
				rsyserr(FERROR_XFER, errno,
					"rename failed for %s (from %s)",
					full_fname(fname), partialptr);
			} else {
				if (remove_source_files
				 || (preserve_hard_links && F_IS_HLINKED(file)))
					send_msg_int(MSG_SUCCESS, ndx);
				handle_partial_dir(partialptr, PDIR_DELETE);
			}
		}
	}
}