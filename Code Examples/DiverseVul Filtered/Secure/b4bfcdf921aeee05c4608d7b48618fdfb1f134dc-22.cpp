static int restore_case_semantics(struct case_semantics_state *state)
{
	state->conn->case_sensitive = state->case_sensitive;
	state->conn->case_preserve = state->case_preserve;
	state->conn->short_case_preserve = state->short_case_preserve;
	return 0;
}