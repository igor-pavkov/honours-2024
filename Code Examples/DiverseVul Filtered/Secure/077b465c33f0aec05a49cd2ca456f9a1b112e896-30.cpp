PJ_DEF(void) pj_scan_get_quote( pj_scanner *scanner,
				int begin_quote, int end_quote, 
				pj_str_t *out)
{
    char beg = (char)begin_quote;
    char end = (char)end_quote;
    pj_scan_get_quotes(scanner, &beg, &end, 1, out);
}