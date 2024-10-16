int URI_FUNC(ParseUri)(URI_TYPE(ParserState) * state, const URI_CHAR * text) {
	if ((state == NULL) || (text == NULL)) {
		return URI_ERROR_NULL;
	}
	return URI_FUNC(ParseUriEx)(state, text, text + URI_STRLEN(text));
}