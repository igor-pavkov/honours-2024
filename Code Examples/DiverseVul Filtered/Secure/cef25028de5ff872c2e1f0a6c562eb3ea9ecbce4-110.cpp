static const URI_CHAR * URI_FUNC(ParseUriReference)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case URI_SET_ALPHA:
		state->uri->scheme.first = first; /* SCHEME BEGIN */
		return URI_FUNC(ParseSegmentNzNcOrScheme2)(state, first + 1, afterLast, memory);

	case URI_SET_DIGIT:
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
	case _UT('.'):
	case _UT('_'):
	case _UT('~'):
	case _UT('-'):
	case _UT('@'):
		state->uri->scheme.first = first; /* SEGMENT BEGIN, ABUSE SCHEME POINTER */
		return URI_FUNC(ParseMustBeSegmentNzNc)(state, first + 1, afterLast, memory);

	case _UT('%'):
		{
			const URI_CHAR * const afterPctEncoded
					= URI_FUNC(ParsePctEncoded)(state, first, afterLast, memory);
			if (afterPctEncoded == NULL) {
				return NULL;
			}
			state->uri->scheme.first = first; /* SEGMENT BEGIN, ABUSE SCHEME POINTER */
			return URI_FUNC(ParseMustBeSegmentNzNc)(state, afterPctEncoded, afterLast, memory);
		}

	case _UT('/'):
		{
			const URI_CHAR * const afterPartHelperTwo
					= URI_FUNC(ParsePartHelperTwo)(state, first + 1, afterLast, memory);
			if (afterPartHelperTwo == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseUriTail)(state, afterPartHelperTwo, afterLast, memory);
		}

	default:
		return URI_FUNC(ParseUriTail)(state, first, afterLast, memory);
	}
}