static const URI_CHAR * URI_FUNC(ParseMustBeSegmentNzNc)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		if (!URI_FUNC(PushPathSegment)(state, state->uri->scheme.first, first, memory)) { /* SEGMENT BOTH */
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		state->uri->scheme.first = NULL; /* Not a scheme, reset */
		return afterLast;
	}

	switch (*first) {
	case _UT('%'):
		{
			const URI_CHAR * const afterPctEncoded
					= URI_FUNC(ParsePctEncoded)(state, first, afterLast, memory);
			if (afterPctEncoded == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseMustBeSegmentNzNc)(state, afterPctEncoded, afterLast, memory);
		}

	case _UT('@'):
	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('*'):
	case _UT(','):
	case _UT(';'):
	case _UT('\''):
	case _UT('+'):
	case _UT('='):
	case _UT('-'):
	case _UT('.'):
	case _UT('_'):
	case _UT('~'):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		return URI_FUNC(ParseMustBeSegmentNzNc)(state, first + 1, afterLast, memory);

	case _UT('/'):
		{
			const URI_CHAR * afterZeroMoreSlashSegs;
			const URI_CHAR * afterSegment;
			if (!URI_FUNC(PushPathSegment)(state, state->uri->scheme.first, first, memory)) { /* SEGMENT BOTH */
				URI_FUNC(StopMalloc)(state, memory);
				return NULL;
			}
			state->uri->scheme.first = NULL; /* Not a scheme, reset */
			afterSegment = URI_FUNC(ParseSegment)(state, first + 1, afterLast, memory);
			if (afterSegment == NULL) {
				return NULL;
			}
			if (!URI_FUNC(PushPathSegment)(state, first + 1, afterSegment, memory)) { /* SEGMENT BOTH */
				URI_FUNC(StopMalloc)(state, memory);
				return NULL;
			}
			afterZeroMoreSlashSegs
					= URI_FUNC(ParseZeroMoreSlashSegs)(state, afterSegment, afterLast, memory);
			if (afterZeroMoreSlashSegs == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseUriTail)(state, afterZeroMoreSlashSegs, afterLast, memory);
		}

	default:
		if (!URI_FUNC(PushPathSegment)(state, state->uri->scheme.first, first, memory)) { /* SEGMENT BOTH */
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		state->uri->scheme.first = NULL; /* Not a scheme, reset */
		return URI_FUNC(ParseUriTail)(state, first, afterLast, memory);
	}
}