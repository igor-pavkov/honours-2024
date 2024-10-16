static int cli_session_setup_gensec_state_destructor(
	struct cli_session_setup_gensec_state *state)
{
	TALLOC_FREE(state->auth_generic);
	data_blob_clear_free(&state->session_key);
	return 0;
}