UriBool URI_FUNC(RemoveDotSegmentsAbsolute)(URI_TYPE(Uri) * uri,
		UriMemoryManager * memory) {
	const UriBool ABSOLUTE = URI_FALSE;
	return URI_FUNC(RemoveDotSegments)(uri, ABSOLUTE, memory);
}