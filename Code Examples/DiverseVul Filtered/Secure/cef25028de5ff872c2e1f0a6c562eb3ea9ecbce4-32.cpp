static URI_INLINE const URI_CHAR * URI_FUNC(ParseAuthority)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		/* "" regname host */
		state->uri->hostText.first = URI_FUNC(SafeToPointTo);
		state->uri->hostText.afterLast = URI_FUNC(SafeToPointTo);
		return afterLast;
	}

	switch (*first) {
	case _UT('['):
		{
			const URI_CHAR * const afterIpLit2
					= URI_FUNC(ParseIpLit2)(state, first + 1, afterLast, memory);
			if (afterIpLit2 == NULL) {
				return NULL;
			}
			state->uri->hostText.first = first + 1; /* HOST BEGIN */
			return URI_FUNC(ParseAuthorityTwo)(state, afterIpLit2, afterLast);
		}

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
		state->uri->userInfo.first = first; /* USERINFO BEGIN */
		return URI_FUNC(ParseOwnHostUserInfoNz)(state, first, afterLast, memory);

	default:
		/* "" regname host */
		state->uri->hostText.first = URI_FUNC(SafeToPointTo);
		state->uri->hostText.afterLast = URI_FUNC(SafeToPointTo);
		return first;
	}
}