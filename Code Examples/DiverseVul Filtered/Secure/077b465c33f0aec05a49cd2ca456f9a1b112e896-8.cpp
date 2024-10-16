PJ_DEF(int) pj_scan_strcmp( pj_scanner *scanner, const char *s, int len)
{
    if (scanner->curptr + len > scanner->end) {
	pj_scan_syntax_err(scanner);
	return -1;
    }
    return strncmp(scanner->curptr, s, len);
}