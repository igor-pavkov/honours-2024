static int cli_full_connection_state_destructor(
	struct cli_full_connection_state *s)
{
	if (s->cli != NULL) {
		cli_shutdown(s->cli);
		s->cli = NULL;
	}
	return 0;
}