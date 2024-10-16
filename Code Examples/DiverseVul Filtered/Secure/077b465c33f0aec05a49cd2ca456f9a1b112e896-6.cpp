PJ_DEF(int) pj_scan_peek_until( pj_scanner *scanner,
				const pj_cis_t *spec, 
				pj_str_t *out)
{
    register char *s = scanner->curptr;

    if (s >= scanner->end) {
	pj_scan_syntax_err(scanner);
	return -1;
    }

    while (PJ_SCAN_CHECK_EOF(s) && !pj_cis_match( spec, *s))
	++s;

    pj_strset3(out, scanner->curptr, s);
    return *s;
}