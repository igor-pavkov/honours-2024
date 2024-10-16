static void NOINLINE retrieve_file_data(FILE *dfp)
{
#if ENABLE_FEATURE_WGET_STATUSBAR || ENABLE_FEATURE_WGET_TIMEOUT
# if ENABLE_FEATURE_WGET_TIMEOUT
	unsigned second_cnt = G.timeout_seconds;
# endif
	struct pollfd polldata;

	polldata.fd = fileno(dfp);
	polldata.events = POLLIN | POLLPRI;
#endif
	if (!(option_mask32 & WGET_OPT_QUIET)) {
		if (G.output_fd == 1)
			fprintf(stderr, "writing to stdout\n");
		else
			fprintf(stderr, "saving to '%s'\n", G.fname_out);
	}
	progress_meter(PROGRESS_START);

	if (G.chunked)
		goto get_clen;

	/* Loops only if chunked */
	while (1) {

#if ENABLE_FEATURE_WGET_STATUSBAR || ENABLE_FEATURE_WGET_TIMEOUT
		/* Must use nonblocking I/O, otherwise fread will loop
		 * and *block* until it reads full buffer,
		 * which messes up progress bar and/or timeout logic.
		 * Because of nonblocking I/O, we need to dance
		 * very carefully around EAGAIN. See explanation at
		 * clearerr() calls.
		 */
		ndelay_on(polldata.fd);
#endif
		while (1) {
			int n;
			unsigned rdsz;

#if ENABLE_FEATURE_WGET_STATUSBAR || ENABLE_FEATURE_WGET_TIMEOUT
			/* fread internally uses read loop, which in our case
			 * is usually exited when we get EAGAIN.
			 * In this case, libc sets error marker on the stream.
			 * Need to clear it before next fread to avoid possible
			 * rare false positive ferror below. Rare because usually
			 * fread gets more than zero bytes, and we don't fall
			 * into if (n <= 0) ...
			 */
			clearerr(dfp);
#endif
			errno = 0;
			rdsz = sizeof(G.wget_buf);
			if (G.got_clen) {
				if (G.content_len < (off_t)sizeof(G.wget_buf)) {
					if ((int)G.content_len <= 0)
						break;
					rdsz = (unsigned)G.content_len;
				}
			}
			n = fread(G.wget_buf, 1, rdsz, dfp);

			if (n > 0) {
				xwrite(G.output_fd, G.wget_buf, n);
#if ENABLE_FEATURE_WGET_STATUSBAR
				G.transferred += n;
#endif
				if (G.got_clen) {
					G.content_len -= n;
					if (G.content_len == 0)
						break;
				}
#if ENABLE_FEATURE_WGET_TIMEOUT
				second_cnt = G.timeout_seconds;
#endif
				goto bump;
			}

			/* n <= 0.
			 * man fread:
			 * If error occurs, or EOF is reached, the return value
			 * is a short item count (or zero).
			 * fread does not distinguish between EOF and error.
			 */
			if (errno != EAGAIN) {
				if (ferror(dfp)) {
					progress_meter(PROGRESS_END);
					bb_simple_perror_msg_and_die(bb_msg_read_error);
				}
				break; /* EOF, not error */
			}

#if ENABLE_FEATURE_WGET_STATUSBAR || ENABLE_FEATURE_WGET_TIMEOUT
			/* It was EAGAIN. There is no data. Wait up to one second
			 * then abort if timed out, or update the bar and try reading again.
			 */
			if (safe_poll(&polldata, 1, 1000) == 0) {
# if ENABLE_FEATURE_WGET_TIMEOUT
				if (second_cnt != 0 && --second_cnt == 0) {
					progress_meter(PROGRESS_END);
					bb_simple_error_msg_and_die("download timed out");
				}
# endif
				/* We used to loop back to poll here,
				 * but there is no great harm in letting fread
				 * to try reading anyway.
				 */
			}
#endif
 bump:
			/* Need to do it _every_ second for "stalled" indicator
			 * to be shown properly.
			 */
			progress_meter(PROGRESS_BUMP);
		} /* while (reading data) */

#if ENABLE_FEATURE_WGET_STATUSBAR || ENABLE_FEATURE_WGET_TIMEOUT
		clearerr(dfp);
		ndelay_off(polldata.fd); /* else fgets can get very unhappy */
#endif
		if (!G.chunked)
			break;

		/* Each chunk ends with "\r\n" - eat it */
		fgets_trim_sanitize(dfp, NULL);
 get_clen:
		/* chunk size format is "HEXNUM[;name[=val]]\r\n" */
		fgets_trim_sanitize(dfp, NULL);
		errno = 0;
		G.content_len = STRTOOFF(G.wget_buf, NULL, 16);
		/*
		 * Had a bug with inputs like "ffffffff0001f400"
		 * smashing the heap later. Ensure >= 0.
		 */
		if (G.content_len < 0 || errno)
			bb_error_msg_and_die("bad chunk length '%s'", G.wget_buf);
		if (G.content_len == 0)
			break; /* all done! */
		G.got_clen = 1;
		/*
		 * Note that fgets may result in some data being buffered in dfp.
		 * We loop back to fread, which will retrieve this data.
		 * Also note that code has to be arranged so that fread
		 * is done _before_ one-second poll wait - poll doesn't know
		 * about stdio buffering and can result in spurious one second waits!
		 */
	}

	/* Draw full bar and free its resources */
	G.chunked = 0;  /* makes it show 100% even for chunked download */
	G.got_clen = 1; /* makes it show 100% even for download of (formerly) unknown size */
	progress_meter(PROGRESS_END);
	if (G.content_len != 0) {
		bb_simple_perror_msg_and_die("connection closed prematurely");
		/* GNU wget says "DATE TIME (NN MB/s) - Connection closed at byte NNN. Retrying." */
	}

	/* If -c failed, we restart from the beginning,
	 * but we do not truncate file then, we do it only now, at the end.
	 * This lets user to ^C if his 99% complete 10 GB file download
	 * failed to restart *without* losing the almost complete file.
	 */
	{
		off_t pos = lseek(G.output_fd, 0, SEEK_CUR);
		if (pos != (off_t)-1)
			ftruncate(G.output_fd, pos);
	}

	if (!(option_mask32 & WGET_OPT_QUIET)) {
		if (G.output_fd == 1)
			fprintf(stderr, "written to stdout\n");
		else
			fprintf(stderr, "'%s' saved\n", G.fname_out);
	}
}