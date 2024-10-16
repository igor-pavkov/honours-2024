int URI_FUNC(ParseSingleUriEx)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		const URI_CHAR ** errorPos) {
    if ((afterLast == NULL) && (first != NULL)) {
		afterLast = first + URI_STRLEN(first);
	}
	return URI_FUNC(ParseSingleUriExMm)(uri, first, afterLast, errorPos, NULL);
}