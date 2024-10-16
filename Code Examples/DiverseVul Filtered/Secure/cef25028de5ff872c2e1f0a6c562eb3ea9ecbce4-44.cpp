static const URI_CHAR * URI_FUNC(ParsePort)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case URI_SET_DIGIT:
		return URI_FUNC(ParsePort)(state, first + 1, afterLast);

	default:
		return first;
	}
}