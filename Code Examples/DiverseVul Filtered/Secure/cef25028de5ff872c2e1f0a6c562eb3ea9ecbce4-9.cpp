static URI_INLINE UriBool URI_FUNC(PushPathSegment)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	URI_TYPE(PathSegment) * segment = memory->calloc(memory, 1, sizeof(URI_TYPE(PathSegment)));
	if (segment == NULL) {
		return URI_FALSE; /* Raises malloc error */
	}
	if (first == afterLast) {
		segment->text.first = URI_FUNC(SafeToPointTo);
		segment->text.afterLast = URI_FUNC(SafeToPointTo);
	} else {
		segment->text.first = first;
		segment->text.afterLast = afterLast;
	}

	/* First segment ever? */
	if (state->uri->pathHead == NULL) {
		/* First segment ever, set head and tail */
		state->uri->pathHead = segment;
		state->uri->pathTail = segment;
	} else {
		/* Append, update tail */
		state->uri->pathTail->next = segment;
		state->uri->pathTail = segment;
	}

	return URI_TRUE; /* Success */
}