static URI_INLINE void URI_FUNC(OnExitPartHelperTwo)(URI_TYPE(ParserState) * state) {
	state->uri->absolutePath = URI_TRUE;
}