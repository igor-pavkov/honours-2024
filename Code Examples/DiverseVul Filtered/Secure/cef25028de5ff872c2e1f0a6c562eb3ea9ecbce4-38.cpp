static URI_INLINE void URI_FUNC(StopSyntax)(URI_TYPE(ParserState) * state,
		const URI_CHAR * errorPos, UriMemoryManager * memory) {
	URI_FUNC(FreeUriMembersMm)(state->uri, memory);
	state->errorPos = errorPos;
	state->errorCode = URI_ERROR_SYNTAX;
}