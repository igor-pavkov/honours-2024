PJ_DEF(int) pj_scan_peek( pj_scanner *scanner,
			  const pj_cis_t *spec, pj_str_t *out)
{
    register char *s = scanner->curptr;

    if (s >= scanner->end) {
	pj_scan_syntax_err(scanner);
	return -1;
    }

    /* Don't need to check EOF with PJ_SCAN_CHECK_EOF(s) */
    while (pj_cis_match(spec, *s))
	++s;

    pj_strset3(out, scanner->curptr, s);
    return *s;
}