static URI_INLINE const URI_CHAR * URI_FUNC(ParseIpLit2)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		URI_FUNC(StopSyntax)(state, first, memory);
		return NULL;
	}

	switch (*first) {
	case _UT('v'):
		{
			const URI_CHAR * const afterIpFuture
					= URI_FUNC(ParseIpFuture)(state, first, afterLast, memory);
			if (afterIpFuture == NULL) {
				return NULL;
			}
			if ((afterIpFuture >= afterLast)
					|| (*afterIpFuture != _UT(']'))) {
				URI_FUNC(StopSyntax)(state, first, memory);
				return NULL;
			}
			return afterIpFuture + 1;
		}

	case _UT(':'):
	case _UT(']'):
	case URI_SET_HEXDIG:
		state->uri->hostData.ip6 = memory->malloc(memory, 1 * sizeof(UriIp6)); /* Freed when stopping on parse error */
		if (state->uri->hostData.ip6 == NULL) {
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		return URI_FUNC(ParseIPv6address2)(state, first, afterLast, memory);

	default:
		URI_FUNC(StopSyntax)(state, first, memory);
		return NULL;
	}
}