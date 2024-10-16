PJ_DEF(void) pj_scan_skip_whitespace( pj_scanner *scanner )
{
    register char *s = scanner->curptr;

    while (PJ_SCAN_IS_SPACE(*s)) {
	++s;
    }

    if (PJ_SCAN_IS_NEWLINE(*s) && (scanner->skip_ws & PJ_SCAN_AUTOSKIP_NEWLINE)) {
	for (;;) {
	    if (*s == '\r') {
		++s;
		if (*s == '\n') ++s;
		++scanner->line;
		scanner->curptr = scanner->start_line = s;
	    } else if (*s == '\n') {
		++s;
		++scanner->line;
		scanner->curptr = scanner->start_line = s;
	    } else if (PJ_SCAN_IS_SPACE(*s)) {
		do {
		    ++s;
		} while (PJ_SCAN_IS_SPACE(*s));
	    } else {
		break;
	    }
	}
    }

    if (PJ_SCAN_IS_NEWLINE(*s) && (scanner->skip_ws & PJ_SCAN_AUTOSKIP_WS_HEADER)==PJ_SCAN_AUTOSKIP_WS_HEADER) {
	/* Check for header continuation. */
	scanner->curptr = s;

	if (*s == '\r') {
	    ++s;
	}
	if (*s == '\n') {
	    ++s;
	}
	scanner->start_line = s;

	if (PJ_SCAN_IS_SPACE(*s)) {
	    register char *t = s;
	    do {
		++t;
	    } while (PJ_SCAN_IS_SPACE(*t));

	    ++scanner->line;
	    scanner->curptr = t;
	}
    } else {
	scanner->curptr = s;
    }
}