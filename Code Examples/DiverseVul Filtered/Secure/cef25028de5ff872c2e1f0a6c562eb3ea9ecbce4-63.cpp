static URI_INLINE const URI_CHAR * URI_FUNC(ParsePathAbsNoLeadSlash)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('@'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		{
			const URI_CHAR * const afterSegmentNz
					= URI_FUNC(ParseSegmentNz)(state, first, afterLast, memory);
			if (afterSegmentNz == NULL) {
				return NULL;
			}
			if (!URI_FUNC(PushPathSegment)(state, first, afterSegmentNz, memory)) { /* SEGMENT BOTH */
				URI_FUNC(StopMalloc)(state, memory);
				return NULL;
			}
			return URI_FUNC(ParseZeroMoreSlashSegs)(state, afterSegmentNz, afterLast, memory);
		}

	default:
		return first;
	}
}