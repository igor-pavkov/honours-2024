static URI_INLINE const URI_CHAR * URI_FUNC(ParsePathRootless)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	const URI_CHAR * const afterSegmentNz
			= URI_FUNC(ParseSegmentNz)(state, first, afterLast, memory);
	if (afterSegmentNz == NULL) {
		return NULL;
	} else {
		if (!URI_FUNC(PushPathSegment)(state, first, afterSegmentNz, memory)) { /* SEGMENT BOTH */
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
	}
	return URI_FUNC(ParseZeroMoreSlashSegs)(state, afterSegmentNz, afterLast, memory);
}