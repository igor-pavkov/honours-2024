PJ_DEF(int) pj_scan_stricmp( pj_scanner *scanner, const char *s, int len)
{
    if (scanner->curptr + len > scanner->end) {
	pj_scan_syntax_err(scanner);
	return -1;
    }
    return pj_ansi_strnicmp(scanner->curptr, s, len);
}