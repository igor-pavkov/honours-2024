PJ_DEF(void) pj_scan_save_state( const pj_scanner *scanner, 
				 pj_scan_state *state)
{
    state->curptr = scanner->curptr;
    state->line = scanner->line;
    state->start_line = scanner->start_line;
}