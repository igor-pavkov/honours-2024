PJ_DEF(void) pj_scan_restore_state( pj_scanner *scanner, 
				    pj_scan_state *state)
{
    scanner->curptr = state->curptr;
    scanner->line = state->line;
    scanner->start_line = state->start_line;
}