void URI_FUNC(ResetUri)(URI_TYPE(Uri) * uri) {
	if (uri == NULL) {
		return;
	}
	memset(uri, 0, sizeof(URI_TYPE(Uri)));
}