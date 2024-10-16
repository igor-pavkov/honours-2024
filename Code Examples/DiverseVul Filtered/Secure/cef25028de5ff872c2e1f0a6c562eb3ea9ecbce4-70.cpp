static URI_INLINE const URI_CHAR * URI_FUNC(ParseUriTail)(
		URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('#'):
		{
			const URI_CHAR * const afterQueryFrag = URI_FUNC(ParseQueryFrag)(state, first + 1, afterLast, memory);
			if (afterQueryFrag == NULL) {
				return NULL;
			}
			state->uri->fragment.first = first + 1; /* FRAGMENT BEGIN */
			state->uri->fragment.afterLast = afterQueryFrag; /* FRAGMENT END */
			return afterQueryFrag;
		}

	case _UT('?'):
		{
			const URI_CHAR * const afterQueryFrag
					= URI_FUNC(ParseQueryFrag)(state, first + 1, afterLast, memory);
			if (afterQueryFrag == NULL) {
				return NULL;
			}
			state->uri->query.first = first + 1; /* QUERY BEGIN */
			state->uri->query.afterLast = afterQueryFrag; /* QUERY END */
			return URI_FUNC(ParseUriTailTwo)(state, afterQueryFrag, afterLast, memory);
		}

	default:
		return first;
	}
}