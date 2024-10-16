static int URI_FUNC(ParseUriExMm)(URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	const URI_CHAR * afterUriReference;
	URI_TYPE(Uri) * uri;

	/* Check params */
	if ((state == NULL) || (first == NULL) || (afterLast == NULL)) {
		return URI_ERROR_NULL;
	}
	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	uri = state->uri;

	/* Init parser */
	URI_FUNC(ResetParserStateExceptUri)(state);
	URI_FUNC(ResetUri)(uri);

	/* Parse */
	afterUriReference = URI_FUNC(ParseUriReference)(state, first, afterLast, memory);
	if (afterUriReference == NULL) {
		return state->errorCode;
	}
	if (afterUriReference != afterLast) {
		URI_FUNC(StopSyntax)(state, afterUriReference, memory);
		return state->errorCode;
	}
	return URI_SUCCESS;
}