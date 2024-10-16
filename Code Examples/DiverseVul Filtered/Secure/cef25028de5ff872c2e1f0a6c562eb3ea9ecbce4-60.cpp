static URI_INLINE UriBool URI_FUNC(OnExitSegmentNzNcOrScheme2)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		UriMemoryManager * memory) {
	if (!URI_FUNC(PushPathSegment)(state, state->uri->scheme.first, first, memory)) { /* SEGMENT BOTH */
		return URI_FALSE; /* Raises malloc error*/
	}
	state->uri->scheme.first = NULL; /* Not a scheme, reset */
	return URI_TRUE; /* Success */
}