UriBool URI_FUNC(_TESTING_ONLY_ParseIpSix)(const URI_CHAR * text) {
	UriMemoryManager * const memory = &defaultMemoryManager;
	URI_TYPE(Uri) uri;
	URI_TYPE(ParserState) parser;
	const URI_CHAR * const afterIpSix = text + URI_STRLEN(text);
	const URI_CHAR * res;

	URI_FUNC(ResetUri)(&uri);
	parser.uri = &uri;
	URI_FUNC(ResetParserStateExceptUri)(&parser);
	parser.uri->hostData.ip6 = memory->malloc(memory, 1 * sizeof(UriIp6));
	res = URI_FUNC(ParseIPv6address2)(&parser, text, afterIpSix, memory);
	URI_FUNC(FreeUriMembersMm)(&uri, memory);
	return res == afterIpSix ? URI_TRUE : URI_FALSE;
}