static const URI_CHAR * URI_FUNC(ParseOwnPortUserInfo)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		if (!URI_FUNC(OnExitOwnPortUserInfo)(state, first, memory)) {
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		return afterLast;
	}

	switch (*first) {
	/* begin sub-delims */
	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('\''):
	case _UT('('):
	case _UT(')'):
	case _UT('*'):
	case _UT('+'):
	case _UT(','):
	case _UT(';'):
	case _UT('='):
	/* end sub-delims */
	/* begin unreserved (except alpha and digit) */
	case _UT('-'):
	case _UT('.'):
	case _UT('_'):
	case _UT('~'):
	/* end unreserved (except alpha and digit) */
	case _UT(':'):
	case URI_SET_ALPHA:
		state->uri->hostText.afterLast = NULL; /* Not a host, reset */
		state->uri->portText.first = NULL; /* Not a port, reset */
		return URI_FUNC(ParseOwnUserInfo)(state, first + 1, afterLast, memory);

	case URI_SET_DIGIT:
		return URI_FUNC(ParseOwnPortUserInfo)(state, first + 1, afterLast, memory);

	case _UT('%'):
		state->uri->portText.first = NULL; /* Not a port, reset */
		{
			const URI_CHAR * const afterPct
					= URI_FUNC(ParsePctEncoded)(state, first, afterLast, memory);
			if (afterPct == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseOwnUserInfo)(state, afterPct, afterLast, memory);
		}

	case _UT('@'):
		state->uri->hostText.afterLast = NULL; /* Not a host, reset */
		state->uri->portText.first = NULL; /* Not a port, reset */
		state->uri->userInfo.afterLast = first; /* USERINFO END */
		state->uri->hostText.first = first + 1; /* HOST BEGIN */
		return URI_FUNC(ParseOwnHost)(state, first + 1, afterLast, memory);

	default:
		if (!URI_FUNC(OnExitOwnPortUserInfo)(state, first, memory)) {
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		return first;
	}
}