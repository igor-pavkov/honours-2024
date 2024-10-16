UriBool URI_FUNC(FixAmbiguity)(URI_TYPE(Uri) * uri,
		UriMemoryManager * memory) {
	URI_TYPE(PathSegment) * segment;

	if (	/* Case 1: absolute path, empty first segment */
			(uri->absolutePath
			&& (uri->pathHead != NULL)
			&& (uri->pathHead->text.afterLast == uri->pathHead->text.first))

			/* Case 2: relative path, empty first and second segment */
			|| (!uri->absolutePath
			&& (uri->pathHead != NULL)
			&& (uri->pathHead->next != NULL)
			&& (uri->pathHead->text.afterLast == uri->pathHead->text.first)
			&& (uri->pathHead->next->text.afterLast == uri->pathHead->next->text.first))) {
		/* NOOP */
	} else {
		return URI_TRUE;
	}

	segment = memory->malloc(memory, 1 * sizeof(URI_TYPE(PathSegment)));
	if (segment == NULL) {
		return URI_FALSE; /* Raises malloc error */
	}

	/* Insert "." segment in front */
	segment->next = uri->pathHead;
	segment->text.first = URI_FUNC(ConstPwd);
	segment->text.afterLast = URI_FUNC(ConstPwd) + 1;
	uri->pathHead = segment;
	return URI_TRUE;
}