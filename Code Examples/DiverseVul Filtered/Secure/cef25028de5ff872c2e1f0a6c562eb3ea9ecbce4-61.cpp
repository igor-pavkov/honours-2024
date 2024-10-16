static const URI_CHAR * URI_FUNC(ParseOwnHost2)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		if (!URI_FUNC(OnExitOwnHost2)(state, first, memory)) {
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
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
	case _UT(';'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		{
			const URI_CHAR * const afterPctSubUnres
					= URI_FUNC(ParsePctSubUnres)(state, first, afterLast, memory);
			if (afterPctSubUnres == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseOwnHost2)(state, afterPctSubUnres, afterLast, memory);
		}

	default:
		if (!URI_FUNC(OnExitOwnHost2)(state, first, memory)) {
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		return URI_FUNC(ParseAuthorityTwo)(state, first, afterLast);
	}
}