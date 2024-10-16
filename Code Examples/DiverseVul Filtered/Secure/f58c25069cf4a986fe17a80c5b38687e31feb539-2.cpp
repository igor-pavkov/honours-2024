UriBool URI_FUNC(CopyPath)(URI_TYPE(Uri) * dest,
		const URI_TYPE(Uri) * source, UriMemoryManager * memory) {
	if (source->pathHead == NULL) {
		/* No path component */
		dest->pathHead = NULL;
		dest->pathTail = NULL;
	} else {
		/* Copy list but not the text contained */
		URI_TYPE(PathSegment) * sourceWalker = source->pathHead;
		URI_TYPE(PathSegment) * destPrev = NULL;
		do {
			URI_TYPE(PathSegment) * cur = memory->malloc(memory, sizeof(URI_TYPE(PathSegment)));
			if (cur == NULL) {
				/* Fix broken list */
				if (destPrev != NULL) {
					destPrev->next = NULL;
				}
				return URI_FALSE; /* Raises malloc error */
			}

			/* From this functions usage we know that *
			 * the dest URI cannot be uri->owner      */
			cur->text = sourceWalker->text;
			if (destPrev == NULL) {
				/* First segment ever */
				dest->pathHead = cur;
			} else {
				destPrev->next = cur;
			}
			destPrev = cur;
			sourceWalker = sourceWalker->next;
		} while (sourceWalker != NULL);
		dest->pathTail = destPrev;
		dest->pathTail->next = NULL;
	}

	dest->absolutePath = source->absolutePath;
	return URI_TRUE;
}