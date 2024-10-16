static struct case_semantics_state *set_posix_case_semantics(TALLOC_CTX *mem_ctx,
						connection_struct *conn)
{
	struct case_semantics_state *result;

	if (!(result = talloc(mem_ctx, struct case_semantics_state))) {
		return NULL;
	}

	result->conn = conn;
	result->case_sensitive = conn->case_sensitive;
	result->case_preserve = conn->case_preserve;
	result->short_case_preserve = conn->short_case_preserve;

	/* Set to POSIX. */
	conn->case_sensitive = True;
	conn->case_preserve = True;
	conn->short_case_preserve = True;

	talloc_set_destructor(result, restore_case_semantics);

	return result;
}