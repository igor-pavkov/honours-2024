static URI_INLINE const URI_CHAR * URI_FUNC(ParseOwnHostUserInfo)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		if (!URI_FUNC(OnExitOwnHostUserInfo)(state, first, memory)) {
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
		return URI_FUNC(ParseOwnHostUserInfoNz)(state, first, afterLast, memory);

	default:
		if (!URI_FUNC(OnExitOwnHostUserInfo)(state, first, memory)) {
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		return first;
	}
}