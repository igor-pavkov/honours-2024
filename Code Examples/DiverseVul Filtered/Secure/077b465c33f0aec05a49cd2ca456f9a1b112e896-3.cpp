PJ_DEF(void) pj_scan_get_quotes(pj_scanner *scanner,
                                const char *begin_quote, const char *end_quote,
                                int qsize, pj_str_t *out)
{
    register char *s = scanner->curptr;
    int qpair = -1;
    int i;

    pj_assert(qsize > 0);

    /* Check and eat the begin_quote. */
    for (i = 0; i < qsize; ++i) {
	if (*s == begin_quote[i]) {
	    qpair = i;
	    break;
	}
    }
    if (qpair == -1) {
	pj_scan_syntax_err(scanner);
	return;
    }
    ++s;

    /* Loop until end_quote is found. 
     */
    do {
	/* loop until end_quote is found. */
	while (PJ_SCAN_CHECK_EOF(s) && *s != '\n' && *s != end_quote[qpair]) {
	    ++s;
	}

	/* check that no backslash character precedes the end_quote. */
	if (*s == end_quote[qpair]) {
	    if (*(s-1) == '\\') {
		char *q = s-2;
		char *r = s-2;

		while (r != scanner->begin && *r == '\\') {
		    --r;
		}
		/* break from main loop if we have odd number of backslashes */
		if (((unsigned)(q-r) & 0x01) == 1) {
		    break;
		}
		++s;
	    } else {
		/* end_quote is not preceeded by backslash. break now. */
		break;
	    }
	} else {
	    /* loop ended by non-end_quote character. break now. */
	    break;
	}
    } while (1);

    /* Check and eat the end quote. */
    if (*s != end_quote[qpair]) {
	pj_scan_syntax_err(scanner);
	return;
    }
    ++s;

    pj_strset3(out, scanner->curptr, s);

    scanner->curptr = s;

    if (PJ_SCAN_IS_PROBABLY_SPACE(*s) && scanner->skip_ws) {
	pj_scan_skip_whitespace(scanner);
    }
}