int URI_FUNC(ParseSingleUri)(URI_TYPE(Uri) * uri, const URI_CHAR * text,
		const URI_CHAR ** errorPos) {
	return URI_FUNC(ParseSingleUriEx)(uri, text, NULL, errorPos);
}