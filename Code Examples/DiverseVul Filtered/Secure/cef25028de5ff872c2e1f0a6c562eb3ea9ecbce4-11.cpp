static const URI_CHAR * URI_FUNC(ParseHexZero)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case URI_SET_HEXDIG:
		return URI_FUNC(ParseHexZero)(state, first + 1, afterLast);

	default:
		return first;
	}
}