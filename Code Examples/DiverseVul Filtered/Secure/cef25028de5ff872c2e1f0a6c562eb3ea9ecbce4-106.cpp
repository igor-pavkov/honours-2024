static const URI_CHAR * URI_FUNC(ParsePathAbsEmpty)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('/'):
		{
			const URI_CHAR * const afterSegment
					= URI_FUNC(ParseSegment)(state, first + 1, afterLast, memory);
			if (afterSegment == NULL) {
				return NULL;
			}
			if (!URI_FUNC(PushPathSegment)(state, first + 1, afterSegment, memory)) { /* SEGMENT BOTH */
				URI_FUNC(StopMalloc)(state, memory);
				return NULL;
			}
			return URI_FUNC(ParsePathAbsEmpty)(state, afterSegment, afterLast, memory);
		}

	default:
		return first;
	}
}