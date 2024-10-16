int URI_FUNC(ParseUriEx)(URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast) {
	return URI_FUNC(ParseUriExMm)(state, first, afterLast, NULL);
}