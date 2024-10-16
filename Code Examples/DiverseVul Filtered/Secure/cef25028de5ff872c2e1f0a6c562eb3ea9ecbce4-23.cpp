static URI_INLINE void URI_FUNC(StopMalloc)(URI_TYPE(ParserState) * state, UriMemoryManager * memory) {
	URI_FUNC(FreeUriMembersMm)(state->uri, memory);
	state->errorPos = NULL;
	state->errorCode = URI_ERROR_MALLOC;
}