static const URI_CHAR * URI_FUNC(ParseOwnHostUserInfoNz)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		URI_FUNC(StopSyntax)(state, first, memory);
		return NULL;
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
			return URI_FUNC(ParseOwnHostUserInfo)(state, afterPctSubUnres, afterLast, memory);
		}

	case _UT(':'):
		state->uri->hostText.afterLast = first; /* HOST END */
		state->uri->portText.first = first + 1; /* PORT BEGIN */
		return URI_FUNC(ParseOwnPortUserInfo)(state, first + 1, afterLast, memory);

	case _UT('@'):
		state->uri->userInfo.afterLast = first; /* USERINFO END */
		state->uri->hostText.first = first + 1; /* HOST BEGIN */
		return URI_FUNC(ParseOwnHost)(state, first + 1, afterLast, memory);

	default:
		URI_FUNC(StopSyntax)(state, first, memory);
		return NULL;
	}
}