UriBool URI_FUNC(RemoveDotSegments)(URI_TYPE(Uri) * uri,
		UriBool relative, UriMemoryManager * memory) {
	if (uri == NULL) {
		return URI_TRUE;
	}
	return URI_FUNC(RemoveDotSegmentsEx)(uri, relative, uri->owner, memory);
}