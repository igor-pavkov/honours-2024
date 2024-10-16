static URI_INLINE const URI_CHAR * URI_FUNC(ParsePartHelperTwo)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		URI_FUNC(OnExitPartHelperTwo)(state);
		return afterLast;
	}

	switch (*first) {
	case _UT('/'):
		{
			const URI_CHAR * const afterAuthority
					= URI_FUNC(ParseAuthority)(state, first + 1, afterLast, memory);
			const URI_CHAR * afterPathAbsEmpty;
			if (afterAuthority == NULL) {
				return NULL;
			}
			afterPathAbsEmpty = URI_FUNC(ParsePathAbsEmpty)(state, afterAuthority, afterLast, memory);

			URI_FUNC(FixEmptyTrailSegment)(state->uri, memory);

			return afterPathAbsEmpty;
		}

	default:
		URI_FUNC(OnExitPartHelperTwo)(state);
		return URI_FUNC(ParsePathAbsNoLeadSlash)(state, first, afterLast, memory);
	}
}