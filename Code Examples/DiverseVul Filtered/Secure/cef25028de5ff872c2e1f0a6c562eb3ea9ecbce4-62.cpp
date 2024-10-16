UriBool URI_FUNC(_TESTING_ONLY_ParseIpFour)(const URI_CHAR * text) {
	unsigned char octets[4];
	int res = URI_FUNC(ParseIpFourAddress)(octets, text, text + URI_STRLEN(text));
	return (res == URI_SUCCESS) ? URI_TRUE : URI_FALSE;
}