PJ_DEF(int) pj_scan_stricmp_alnum( pj_scanner *scanner, const char *s, 
				   int len)
{
    if (scanner->curptr + len > scanner->end) {
	pj_scan_syntax_err(scanner);
	return -1;
    }
    return strnicmp_alnum(scanner->curptr, s, len);
}