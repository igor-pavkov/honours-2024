int URI_FUNC(ParseSingleUriExMm)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		const URI_CHAR ** errorPos, UriMemoryManager * memory) {
	URI_TYPE(ParserState) state;
	int res;

	/* Check params */
	if ((uri == NULL) || (first == NULL) || (afterLast == NULL)) {
		return URI_ERROR_NULL;
	}
	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	state.uri = uri;

	res = URI_FUNC(ParseUriExMm)(&state, first, afterLast, memory);

	if (res != URI_SUCCESS) {
		if (errorPos != NULL) {
			*errorPos = state.errorPos;
		}
		URI_FUNC(FreeUriMembersMm)(uri, memory);
	}

	return res;
}