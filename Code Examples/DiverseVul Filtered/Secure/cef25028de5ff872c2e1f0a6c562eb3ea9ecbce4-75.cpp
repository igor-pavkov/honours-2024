static URI_INLINE const URI_CHAR * URI_FUNC(ParseAuthorityTwo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT(':'):
		{
			const URI_CHAR * const afterPort = URI_FUNC(ParsePort)(state, first + 1, afterLast);
			if (afterPort == NULL) {
				return NULL;
			}
			state->uri->portText.first = first + 1; /* PORT BEGIN */
			state->uri->portText.afterLast = afterPort; /* PORT END */
			return afterPort;
		}

	default:
		return first;
	}
}