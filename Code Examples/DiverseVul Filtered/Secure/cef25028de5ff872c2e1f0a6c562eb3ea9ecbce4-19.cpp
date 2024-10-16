static const URI_CHAR * URI_FUNC(ParsePctEncoded)(
		URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	if (first >= afterLast) {
		URI_FUNC(StopSyntax)(state, first, memory);
		return NULL;
	}

	/*
	First character has already been
	checked before entering this rule.

	switch (*first) {
	case _UT('%'):
	*/
		if (first + 1 >= afterLast) {
			URI_FUNC(StopSyntax)(state, first + 1, memory);
			return NULL;
		}

		switch (first[1]) {
		case URI_SET_HEXDIG:
			if (first + 2 >= afterLast) {
				URI_FUNC(StopSyntax)(state, first + 2, memory);
				return NULL;
			}

			switch (first[2]) {
			case URI_SET_HEXDIG:
				return first + 3;

			default:
				URI_FUNC(StopSyntax)(state, first + 2, memory);
				return NULL;
			}

		default:
			URI_FUNC(StopSyntax)(state, first + 1, memory);
			return NULL;
		}

	/*
	default:
		URI_FUNC(StopSyntax)(state, first, memory);
		return NULL;
	}
	*/
}