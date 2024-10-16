void URI_FUNC(FixEmptyTrailSegment)(URI_TYPE(Uri) * uri,
		UriMemoryManager * memory) {
	/* Fix path if only one empty segment */
	if (!uri->absolutePath
			&& !URI_FUNC(IsHostSet)(uri)
			&& (uri->pathHead != NULL)
			&& (uri->pathHead->next == NULL)
			&& (uri->pathHead->text.first == uri->pathHead->text.afterLast)) {
		memory->free(memory, uri->pathHead);
		uri->pathHead = NULL;
		uri->pathTail = NULL;
	}
}