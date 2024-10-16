PJ_DEF(void) pj_scan_init( pj_scanner *scanner, char *bufstart, 
			   pj_size_t buflen, unsigned options, 
			   pj_syn_err_func_ptr callback )
{
    PJ_CHECK_STACK();

    scanner->begin = scanner->curptr = bufstart;
    scanner->end = bufstart + buflen;
    scanner->line = 1;
    scanner->start_line = scanner->begin;
    scanner->callback = callback;
    scanner->skip_ws = options;

    if (scanner->skip_ws) 
	pj_scan_skip_whitespace(scanner);
}