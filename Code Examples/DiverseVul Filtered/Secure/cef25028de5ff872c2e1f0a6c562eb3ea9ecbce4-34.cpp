static URI_INLINE const URI_CHAR * URI_FUNC(ParseSegmentNz)(
		URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	const URI_CHAR * const afterPchar
			= URI_FUNC(ParsePchar)(state, first, afterLast, memory);
	if (afterPchar == NULL) {
		return NULL;
	}
	return URI_FUNC(ParseSegment)(state, afterPchar, afterLast, memory);
}