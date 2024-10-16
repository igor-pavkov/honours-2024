PJ_DEF(int) pj_scan_peek_n( pj_scanner *scanner,
			     pj_size_t len, pj_str_t *out)
{
    char *endpos = scanner->curptr + len;

    if (endpos > scanner->end) {
	pj_scan_syntax_err(scanner);
	return -1;
    }

    pj_strset(out, scanner->curptr, len);
    return *endpos;
}